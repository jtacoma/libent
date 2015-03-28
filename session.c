#include "ent.h"
#include "model.h"
#include "table.h"
#include "column.h"

#include <stdlib.h>

struct ent_session
{
	struct ent_model * model;
	bool locked;
};

struct ent_session * ent_session_alloc (struct ent_model * m)
{
	struct ent_session * s = calloc (1, sizeof (*s));

	if (s)
	{
		s->model = m;
	}

	return s;
}

void ent_session_free (struct ent_session * s)
{
	free (s);
}

struct ent_table * ent_session_table (struct ent_session * s,
                                      char const * table_name)
{
	if (s->locked)
	{
		return NULL;
	}

	return ent_model_get (s->model, table_name);
}

struct ent_column const *
ent_session_column_r (struct ent_session * s,
                      struct ent_table * table,
                      char const * column_name,
                      char const * type_name)
{
	if (s->locked)
	{
		return NULL;
	}

	return ent_table_column (table, column_name, type_name);
}

struct ent_column *
ent_session_column_w (struct ent_session * s,
                      struct ent_table * table,
                      char const * column_name,
                      char const * type_name)
{
	if (s->locked)
	{
		return NULL;
	}

	struct ent_column * c = ent_table_column (table, column_name, type_name);

	if (!c)
	{
		c = ent_table_add_column (table, column_name, type_name);
	}
	else
	{
		// TODO: make sure it's up to date e.g. w.r.t. len
	}

	return c;
}

struct ent_column *
ent_session_column_a (struct ent_session * s,
                      struct ent_table * table,
                      char const * column_name,
                      char const * type_name)
{
	if (s->locked)
	{
		return NULL;
	}

	struct ent_column * c = ent_table_column (table, column_name, type_name);
	if (!c)
	{
		c = ent_table_add_column (table, column_name, type_name);
	}
	else
	{
		// TODO: make sure it's up to date e.g. w.r.t. len
	}

	return c;
}

int ent_session_lock (struct ent_session * s)
{
	s->locked = true;
	return 0;
}

size_t ent_session_table_len (struct ent_session * s,
                              struct ent_table * table)
{
	if (!s->locked)
	{
		return 0;
	}

	return ent_table_len (table);
}

int ent_session_table_grow (struct ent_session * s,
                            struct ent_table * table,
                            size_t add_size)
{
	if (!s->locked)
	{
		return -1;
	}

	(void)s;
	(void)table;
	(void)add_size;//TODO
	return -1;
}

