#include "ent.h"
#include "table.h"
#include "rlist.h"
#include "column.h"

#include <stdlib.h>
#include <string.h>

struct ent_table
{
	size_t len;
	char ** column_names;
	struct ent_column ** columns;
	size_t columns_len;
	int refcount;
};

struct ent_table *
ent_table_alloc (
    size_t len)
{
	struct ent_table * table = calloc (1, sizeof (*table));

	if (table)
	{
		table->len = len;
	}

	return table;
}

void
ent_table_incref (
    struct ent_table *t)
{
	t->refcount += 1;
}

void
ent_table_decref (
    struct ent_table * t)
{
	if (t && --t->refcount < 0)
	{
		size_t columns_len = t->columns_len;

		if (columns_len)
		{
			for (size_t i = 0; i < columns_len; ++i)
			{
				free (t->column_names[i]);
				ent_column_free (t->columns[i]);
			}

			free (t->column_names);
			free (t->columns);
		}

		free (t);
	}
}

size_t
ent_table_len (
    struct ent_table const * table)
{
	if (!table)
	{
		return 0;
	}

	return table->len;
}

bool
ent_table_has_column_name (
    struct ent_table * table,
    char const * name)
{
	if (! (table && name))
	{
		return false;
	}

	for (size_t i = 0; i < table->columns_len; ++i)
	{
		if (strcmp (table->column_names[i], name) == 0)
		{
			return true;
		}
	}

	return false;
}

struct ent_column *
ent_table_add_column (
    struct ent_table * table,
    char const * name,
    size_t width)
{
	if (! (table && name && width))
	{
		return NULL;
	}

	if (ent_table_has_column_name (table, name))
	{
		return NULL;
	}

	struct ent_column ** newcolumns =
	    realloc (table->columns,
	             sizeof (*table->columns) * (table->columns_len + 1));

	if (!newcolumns)
	{
		return NULL; // out of memory
	}

	table->columns = newcolumns;
	newcolumns[table->columns_len] = NULL;

	char ** newnames =
	    realloc (table->column_names,
	             (sizeof (*newnames)) * (table->columns_len + 1));

	if (!newnames)
	{
		// table->columns: is bigger and zero-padded, no problem
		return NULL; // out of memory
	}

	table->column_names = newnames;
	newnames[table->columns_len] = NULL;

	size_t name_size = strlen (name) + 1;
	newnames[table->columns_len] = malloc (name_size);

	if (!newnames[table->columns_len])
	{
		// table->columns: is bigger and zero-padded, no problem
		// table->column_names: is bigger and zero-padded, no problem
		return NULL; // out of memory
	}

	strncpy (newnames[table->columns_len], name, name_size);
	newcolumns[table->columns_len] = ent_column_alloc (width);

	if (!newcolumns[table->columns_len])
	{
		// table->columns: is bigger and zero-padded, no problem
		// table->column_names: is bigger and zero-padded, no problem
		newnames[table->columns_len] = NULL;
		return NULL; // out of memory
	}

	if (table->len && ent_column_set_len (newcolumns[table->columns_len], table->len) == -1)
	{
		free (newnames[table->columns_len]);
		newnames[table->columns_len] = NULL;
		return NULL; // out of memory
	}

	return table->columns[table->columns_len++];
}

struct ent_column *
ent_table_column (
    struct ent_table * table,
    char const * name,
    size_t width)
{
	if (! (table && name && width))
	{
		return NULL;
	}

	for (size_t i = 0; i < table->columns_len; ++i)
	{
		if (strcmp (table->column_names[i], name) == 0)
		{
			if (width != ent_column_width (table->columns[i]))
			{
				return NULL;
			}

			return table->columns[i];
		}
	}

	return NULL;
}

int
ent_table_delete (
    struct ent_table * table,
    struct ent_rlist const * rlist)
{
	if (! (table && rlist))
	{
		return -1;
	}

	struct ent_rlist * keep = ent_rlist_alloc();
	size_t del_ranges_len = 0;
	struct ent_rlist_range const * del_ranges =
	    ent_rlist_ranges (rlist, &del_ranges_len);

	size_t columns_len = table->columns_len;
	struct ent_column ** newcolumns =
	    calloc (columns_len, sizeof (*newcolumns));

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
		newcolumns[i] = ent_column_alloc (ent_column_width (table->columns[i]));

		if (!newcolumns[i] || ent_column_set_len (newcolumns[i], table->len - ent_rlist_len (rlist)))
		{
			if (newcolumns[i])
			{
				++i;
			}

			ent_rlist_free (keep);

			for (size_t k = 0; k < i; ++k)
			{
				ent_column_free (newcolumns[i]);
			}

			free (newcolumns);
			return -1;
		}
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		void * dst = ent_column_ref (newcolumns[i]);
		void const * src = ent_column_get (table->columns[i]);
		size_t width = ent_column_width (newcolumns[i]);
		if (ent_rlist_select (keep, dst, src, width) == -1)
		{
			ent_rlist_free (keep);

			for (size_t k = 0; k < columns_len; ++k)
			{
				ent_column_free (newcolumns[i]);
			}

			free (newcolumns);
			return -1;
		}
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		ent_column_free (table->columns[i]);
	}

	free (table->columns);
	table->columns = newcolumns;
	table->len = ent_rlist_len (rlist);

	ent_rlist_free (keep);
	return 0;
}

int ent_table_grow (struct ent_table * table,
                    size_t add)
{
	if (! (table && add > 0))
	{
		return -1;
	}

	table->len += add;
	for (size_t i = 0; i < table->columns_len; ++i)
	{
		if (ent_column_set_len (table->columns[i], table->len) == -1)
		{
			return -1;
		}
	}

	return 0;
}
