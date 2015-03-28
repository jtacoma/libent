#include "ent.h"
#include "table.h"

#include <string.h>
#include <stdlib.h>

struct ent_model
{
	struct
	{
		char * name;
		struct ent_table * table;
	} * tables;
	size_t tables_len;
};

struct ent_model * ent_model_alloc()
{
	return calloc (1, sizeof (struct ent_model));
}

void ent_model_free (struct ent_model * m)
{
	if (m)
	{
		if (m->tables)
		{
			for (size_t i = 0; i < m->tables_len; ++i)
			{
				ent_table_free (m->tables[i].table);
				free (m->tables[i].name);
			}

			free (m->tables);
		}
		free (m);
	}
}

bool ent_model_has (struct ent_model * m, char const * table_name)
{
	for (size_t i = 0; i < m->tables_len; ++i)
	{
		if (strcmp (m->tables[i].name, table_name) == 0)
		{
			return true;
		}
	}
	return false;
}

struct ent_table * ent_model_get (struct ent_model * m, char const * table_name)
{
	for (size_t i = 0; i < m->tables_len; ++i)
	{
		if (strcmp (m->tables[i].name, table_name) == 0)
		{
			return m->tables[i].table;
		}
	}

	size_t name_size = strlen (table_name) + 1;
	char * newname = malloc (name_size);
	if (!newname)
	{
		return NULL;
	}
	memcpy (newname, table_name, name_size);

	struct ent_table * t = ent_table_alloc (0);
	if (!t)
	{
		free (newname);
		return NULL;
	}

	void * newtables = realloc (m->tables,
	                            sizeof (*m->tables) * (m->tables_len + 1));
	if (!newtables)
	{
		free (newname);
		ent_table_free (t);
		return NULL;
	}

	m->tables = newtables;
	m->tables_len += 1;
	m->tables[m->tables_len - 1].table = t;
	m->tables[m->tables_len - 1].name = newname;
	return m->tables[m->tables_len - 1].table;
}
