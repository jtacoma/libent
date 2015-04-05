#include "ent.h"
#include "alloc.h"
#include "table.h"
#include "array.h"
#include "processor.h"
#include "session.h"

#include <string.h>
#include <stdlib.h>

struct table
{
	char * name;
	struct ent_table * table;
};

ent_array_typed (struct table, table);

struct ent_model
{
	struct ent_table_array * tables;
};

struct ent_model * ent_model_alloc()
{
	struct ent_model * model = NULL;

	if (ent_alloc ((void**)&model, sizeof (struct ent_model)) == 0)
	{
		*model = (struct ent_model) {0};
		model->tables = ent_table_array_alloc();
		if (!model->tables)
		{
			ent_alloc ((void**)&model, 0);
			return NULL;
		}
	}

	return model;
}

void ent_model_free (struct ent_model * m)
{
	if (m)
	{
		if (m->tables)
		{
			size_t tables_len = ent_table_array_len (m->tables);
			struct table * tables = ent_table_array_get (m->tables);

			for (size_t i = 0; i < tables_len; ++i)
			{
				ent_table_free (tables[i].table);
				ent_alloc ((void**)&tables[i].name, 0);
			}

			ent_table_array_free (m->tables);
		}

		ent_alloc ((void**)&m, 0);
	}
}

bool ent_model_has (struct ent_model * m, char const * table_name)
{
	if (! (m && table_name))
	{
		return false;
	}

	size_t tables_len = ent_table_array_len (m->tables);
	struct table * tables = ent_table_array_get (m->tables);

	for (size_t i = 0; i < tables_len; ++i)
	{
		if (strcmp (tables[i].name, table_name) == 0)
		{
			return true;
		}
	}

	return false;
}

struct ent_table * ent_model_get_const (struct ent_model * m, char const * table_name)
{
	if (! (m && table_name))
	{
		return NULL;
	}

	size_t tables_len = ent_table_array_len (m->tables);
	struct table * tables = ent_table_array_get (m->tables);

	for (size_t i = 0; i < tables_len; ++i)
	{
		if (strcmp (tables[i].name, table_name) == 0)
		{
			ent_table_incref (tables[i].table);
			return tables[i].table;
		}
	}

	size_t name_size = strlen (table_name) + 1;
	char * newname = NULL;

	if (ent_alloc ((void**)&newname, name_size) == -1)
	{
		return NULL;
	}

	memcpy (newname, table_name, name_size);

	struct ent_table * t = ent_table_alloc();

	if (!t)
	{
		ent_alloc ((void**)&newname, 0);
		return NULL;
	}

	if (ent_table_array_set_len (m->tables, tables_len + 1) == -1)
	{
		ent_alloc ((void**)&newname, 0);
		ent_table_free (t);
		return NULL;
	}

	tables = ent_table_array_get (m->tables);
	tables[tables_len].table = t;
	tables[tables_len].name = newname;
	ent_table_incref (t);
	return t;
}
