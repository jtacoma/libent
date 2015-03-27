#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_table
{
	size_t len;
	char ** column_names;
	struct ent_column ** columns;
	size_t columns_len;
};

struct ent_table * ent_table_alloc (size_t len)
{
	struct ent_table * table = ent_realloc_carray (NULL, 1,
	                           sizeof (*table),
	                           true);

	if (table != NULL)
	{
		table->len = len;
	}

	return table;
}

void ent_table_free (struct ent_table * table)
{
	size_t columns_len = table->columns_len;

	if (columns_len)
	{
		for (size_t i = 0; i < columns_len; ++i)
		{
			ent_realloc_free (table->column_names[i]);
			ent_column_free (table->columns[i]);
		}

		ent_realloc_free (table->column_names);
		ent_realloc_free (table->columns);
	}

	ent_realloc_free (table);
}

size_t ent_table_len (struct ent_table const * table)
{
	return table->len;
}

bool ent_table_has_column_name (struct ent_table * table, char const * name)
{
	for (size_t i = 0; i < table->columns_len; ++i)
	{
		if (strcmp (table->column_names[i], name) == 0)
		{
			return true;
		}
	}

	return false;
}

struct ent_column * ent_table_add_column (struct ent_table * table,
        char const * name, char const * type)
{
	if (ent_table_has_column_name (table, name))
	{
		return NULL;
	}

	struct ent_column ** newcolumns = ent_realloc_array (table->columns,
	                                  table->columns_len + 1,
	                                  true);

	if (newcolumns == NULL)
	{
		return NULL; // out of memory
	}

	table->columns = newcolumns;

	char ** newnames = ent_realloc_array (table->column_names,
	                                      table->columns_len + 1,
	                                      true);

	if (newnames == NULL)
	{
		// table->columns: is bigger and zero-padded, no problem
		return NULL; // out of memory
	}

	table->column_names = newnames;

	size_t name_len = strlen (name) + 1;
	newnames[table->columns_len] = ent_realloc_carray (NULL, name_len, sizeof (char), false);

	if (newnames[table->columns_len] == NULL)
	{
		// table->columns: is bigger and zero-padded, no problem
		// table->column_names: is bigger and zero-padded, no problem
		return NULL; // out of memory
	}

	strncpy (newnames[table->columns_len], name, name_len);

	newcolumns[table->columns_len] = ent_column_alloc (type, table->len);

	if (newcolumns[table->columns_len] == NULL)
	{
		// table->columns: is bigger and zero-padded, no problem
		// table->column_names: is bigger and zero-padded, no problem
		newnames[table->columns_len] =
		    ent_realloc_free (newnames[table->columns_len]);
		return NULL; // out of memory
	}

	table->columns_len += 1;
	return table->columns[table->columns_len - 1];
}

struct ent_column * ent_table_column (struct ent_table * table,
                                      char const * name, char const * type)
{
	for (size_t i = 0; i < table->columns_len; ++i)
	{
		if (strcmp (table->column_names[i], name) == 0)
		{
			if (strcmp (ent_column_typename (table->columns[i]), type) != 0)
			{
				return NULL;
			}
			return table->columns[i];
		}
	}

	return NULL;
}

int ent_table_delete (struct ent_table * table,
                      struct ent_rlist const * rlist)
{
	size_t columns_len = table->columns_len;
	struct ent_column ** newcolumns = ent_realloc_carray (NULL,
	                                  columns_len,
	                                  sizeof (*newcolumns),
	                                  true);

	struct ent_rlist * keep = ent_rlist_alloc();
	size_t del_ranges_len = 0;
	struct ent_rlist_range const *del_ranges = ent_rlist_ranges (rlist, &del_ranges_len);

	for (size_t i = 0; i < del_ranges_len; ++i)
	{
		if (del_ranges[i].begin == 0)
		{
			continue;
		}
		size_t begin = i == 0 ? 0 : del_ranges[i - 1].end;
		size_t end = del_ranges[i].begin;
		ent_rlist_append (keep, begin, end);
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		char const * typename = ent_column_typename (table->columns[i]);
		newcolumns[i] = ent_column_alloc (typename, table->len - ent_rlist_len (rlist));
		if (newcolumns[i] == NULL)
		{
			ent_rlist_free (keep);
			for (size_t k = 0; k < i; ++k)
			{
				ent_column_free (newcolumns[i]);
			}
			ent_realloc_free (newcolumns);
			return -1;
		}
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		if (ent_column_select (newcolumns[i], table->columns[i], keep) == -1)
		{
			ent_rlist_free (keep);
			for (size_t k = 0; k < columns_len; ++k)
			{
				ent_column_free (newcolumns[i]);
			}
			ent_realloc_free (newcolumns);
			return -1;
		}
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		ent_column_free (table->columns[i]);
	}

	ent_realloc_free (table->columns);
	table->columns = newcolumns;
	table->len = ent_rlist_len (rlist);

	ent_rlist_free (keep);
	return 0;
}
