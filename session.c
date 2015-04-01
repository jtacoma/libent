#include "ent.h"
#include "model.h"
#include "table.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define table_zero (256)
#define column_zero (INT_MAX/4)

struct insert
{
	struct ent_table * dst;
	struct ent_table * src;
};

enum mode
{
	MODE_CREATE = 1 << 0,
	MODE_READ = 1 << 1,
	MODE_UPDATE = 1 << 2,
	MODE_DELETE = 1 << 3,
};

struct column_info
{
	char * name;
	size_t width;
};

struct ent_session
{
	struct ent_model * model;
	bool locked;
	size_t tables_len;
	struct ent_table ** tables;
	size_t columns_len;
	struct column_info * columns;
	size_t inserts_len;
	struct insert * inserts;
};

struct ent_session *
ent_session_alloc (
    struct ent_model * m)
{
	if (!m)
	{
		return NULL;
	}

	struct ent_session * s = malloc (sizeof (*s));

	if (s)
	{
		*s = (struct ent_session) {.model = m};
	}

	return s;
}

void
ent_session_free (
    struct ent_session * s)
{
	if (s)
	{
		// TODO: separate "commit" from "deallocate"
		// TODO: concatenate "insert" tables to their destinations
		/*
		for (size_t i = 0; i < s->inserts_len; ++i)
		{
			size_t start = ent_table_len (s->inserts[i].dst);
			ent_table_grow (
			    s->inserts[i].dst,
			    ent_table_len (s->inserts[i].src));
		}
		*/

		for (size_t i = 0; i < s->tables_len; ++i)
		{
			ent_table_decref (s->tables[i]);
		}

		for (size_t i = 0; i < s->columns_len; ++i)
		{
			free (s->columns[i].name);
		}

		free (s->tables);
		free (s->columns);
		free (s->inserts);
		free (s);
	}
}

int
ent_session_table (
    struct ent_session * s,
    char const * table_name,
    char const * mode)
{
	if (!s || s->locked)
	{
		return -1;
	}

	(void)mode;//TODO: stop ignoring mode

	struct ent_table ** tables = realloc (s->tables, (s->tables_len + 1) * sizeof (*tables));
	if (!tables)
	{
		return -1;
	}
	s->tables = tables;
	tables[s->tables_len] = ent_model_get (s->model, table_name);
	if (!tables[s->tables_len])
	{
		return -1;
	}
	s->tables_len += 1;
	return table_zero - 1 + s->tables_len;
}

int
ent_session_column (
    struct ent_session * s,
    int table,
    char const * column_name,
    size_t width,
    char const * mode)
{
	if (!s || s->locked)
	{
		return -1;
	}

	(void)mode;//TODO: stop ignoring mode

	size_t index = (size_t)table - table_zero;

	if (s->tables_len <= index)
	{
		return -1;
	}

	struct ent_table * t = s->tables[index];
	struct ent_array * a = ent_table_column (t, column_name, width);

	if (!a)
	{
		a = ent_table_add_column (t, column_name, width);
	}
	else
	{
		// TODO: make sure it's up to date e.g. w.r.t. len
	}

	if (!a)
	{
		return -1;
	}

	struct column_info * columns =
	    realloc (s->columns, (s->columns_len + 1) * sizeof (*columns));

	if (columns == NULL)
	{
		return -1;
	}

	s->columns = columns;
	columns[s->columns_len] = (struct column_info) { .width = width };

	size_t name_len = strlen (column_name) + 1;
	columns[s->columns_len].name = malloc (name_len);
	if (!columns[s->columns_len].name)
	{
		return -1;
	}
	memcpy (columns[s->columns_len].name, column_name, name_len);

	s->columns_len += 1;
	return column_zero - 1 + s->columns_len;
}

int
ent_session_lock (
    struct ent_session * s)
{
	if (!s)
	{
		return -1;
	}

	s->locked = true;
	return 0;
}

size_t
ent_session_table_len (
    struct ent_session * s,
    int table)
{
	if (!s || !s->locked)
	{
		return 0;
	}

	size_t index = (size_t)table - table_zero;

	if (s->tables_len <= index)
	{
		return 0;
	}

	struct ent_table * t = s->tables[index];
	return ent_table_len (t);
}

int
ent_session_table_insert (
    struct ent_session * s,
    int table,
    size_t add)
{
	if (!s || !s->locked || !add)
	{
		return -1;
	}

	size_t index = (size_t)table - table_zero;
	if (s->tables_len <= index)
	{
		return -1;
	}

	struct ent_table * t = ent_table_alloc (add);
	if (t == NULL)
	{
		return -1;
	}

	struct ent_table ** tables = realloc (s->tables, s->tables_len + 1);
	if (tables == NULL)
	{
		ent_table_decref (t);
		return -1;
	}
	s->tables[s->tables_len] = t;

	// TODO: setup appropriate schema based on pre-specified columns

	struct insert * inserts = realloc (s->inserts, s->inserts_len + 1);
	if (inserts == NULL)
	{
		ent_table_decref (t);
		s->tables[s->tables_len] = NULL;
		return -1;
	}
	inserts[s->inserts_len].dst = s->tables[index];
	inserts[s->inserts_len].src = t;
	s->inserts = inserts;
	s->inserts_len += 1;
	s->tables_len += 1;
	return table_zero - 1 + s->tables_len;
}

void *
ent_session_column_write (
    struct ent_session * s,
    int table,
    int column)
{
	if (!s || !s->locked)
	{
		return NULL;
	}

	size_t c_index = (size_t)column - column_zero;

	if (s->columns_len <= c_index)
	{
		return NULL;
	}

	struct column_info const * info = &s->columns[c_index];

	size_t t_index = (size_t)table - table_zero;

	if (s->tables_len <= t_index)
	{
		return NULL;
	}

	struct ent_table * t = s->tables[t_index];

	struct ent_array * a =
	    ent_table_add_column (
	        t,
	        info->name,
	        info->width);

	if (!a)
	{
		return NULL;
	}

	return ent_array_ref (a);
}
