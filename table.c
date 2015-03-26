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
	struct ent_table * table = calloc (1, sizeof (*table));
	table->len = len;
	return table;
}

void ent_table_free (struct ent_table * table)
{
	if (table != NULL)
	{
		size_t columns_len = table->columns_len;
		if (columns_len)
		{
			for (size_t i = 0; i < columns_len; ++i)
			{
				free (table->column_names[i]);
				ent_column_free (table->columns[i]);
			}
			free (table->column_names);
			free (table->columns);
		}
		free (table);
	}
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
	struct ent_column ** newcolumns = realloc (table->columns, sizeof (*newcolumns) * (table->columns_len + 1));
	if (newcolumns == NULL)
	{
		return NULL; // out of memory
	}
	table->columns = newcolumns;

	if (ent_table_has_column_name (table, name))
	{
		return NULL;
	}

	char ** newnames = realloc (table->column_names, sizeof (char*) * (table->columns_len + 1));
	if (newnames == NULL)
	{
		return NULL; // out of memory
	}
	size_t name_len = strlen (name) + 1;
	newnames[table->columns_len] = malloc (name_len);
	strncpy (newnames[table->columns_len], name, name_len);

	newcolumns[table->columns_len] = ent_column_alloc (type, table->len);

	table->column_names = newnames;
	table->columns = newcolumns;
	table->columns_len += 1;
	return table->columns[table->columns_len - 1];
}
