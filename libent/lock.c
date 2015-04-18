#include <assert.h>
#include <stdint.h> // for "array.h"
#include <string.h> // strlen, memcpy

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "lock.h"
#include "table.h"

struct ent_lock
{
	struct ent_array * tables; // struct ent_table **
	struct ent_array * columns; // struct column_info *
	ent_procfunc func;
	void * func_arg;
};

struct ent_lock *
ent_lock_alloc()
{
	struct ent_lock * p = NULL;

	if (ent_alloc ((void**)&p, sizeof (*p)) == 0)
	{
		*p = (struct ent_lock) {0};
		p->tables = ent_array_alloc (sizeof (struct ent_table *));
		if (!p->tables)
		{
			ent_alloc ((void**)&p, 0);
			return NULL;
		}
		p->columns = ent_array_alloc (sizeof (struct column_info));
		if (!p->columns)
		{
			ent_array_free (p->tables);
			ent_alloc ((void**)&p, 0);
			return NULL;
		}
	}

	return p;
}

void
ent_lock_free (
    struct ent_lock * p)
{
	if (p)
	{
		size_t tables_len = ent_array_len (p->tables);
		struct ent_table ** tables = ent_array_get (p->tables);

		for (size_t i = 0; i < tables_len; ++i)
		{
			ent_table_free (tables[i]);
		}

		size_t columns_len = ent_array_len (p->columns);
		struct column_info * columns = ent_array_get (p->columns);

		for (size_t i = 0; i < columns_len; ++i)
		{
			ent_alloc ((void**)&columns[i].name, 0);
		}

		ent_array_free (p->tables);
		ent_array_free (p->columns);
		ent_alloc ((void**)&p, 0);
	}
}

int
ent_lock_for_insert (
    struct ent_lock * p,
    struct ent_table * table)
{
	if (! (p && table))
	{
		return -1;
	}

	size_t tables_len = ent_array_len (p->tables);

	if (ent_array_set_len (p->tables, tables_len + 1) == -1)
	{
		return -1;
	}

	struct ent_table ** tables = ent_array_get (p->tables);

	assert (tables);

	tables[tables_len] = table;
	ent_table_incref (table);

	return (int) tables_len;
}

int
ent_lock_for_delete (
    struct ent_lock * p,
    struct ent_table * table)
{
	if (! (p && table))
	{
		return -1;
	}

	size_t tables_len = ent_array_len (p->tables);

	if (ent_array_set_len (p->tables, tables_len + 1) == -1)
	{
		return -1;
	}

	struct ent_table ** tables = ent_array_get (p->tables);

	assert (tables);

	tables[tables_len] = table;
	ent_table_incref (table);

	return (int) tables_len;
}

int
ent_lock_for_update (
    struct ent_lock * p,
    struct ent_table * table,
    char const * column_name,
    size_t width)
{
	if (!p)
	{
		return -1;
	}

	struct ent_array * array = ent_table_column (
	                               table,
	                               column_name,
	                               width);

	if (!array)
	{
		return -1;
	}

	size_t columns_len = ent_array_len (p->columns);

	if (ent_array_set_len (p->columns, columns_len + 1) == -1)
	{
		return -1;
	}

	struct column_info * columns = ent_array_get (p->columns);

	columns[columns_len] = (struct column_info) { .width = width };

	size_t name_len = strlen (column_name) + 1;
	columns[columns_len].name = NULL;

	if (ent_alloc ((void**)&columns[columns_len].name , name_len) == -1)
	{
		return -1;
	}

	memcpy (columns[columns_len].name, column_name, name_len);

	return (int) columns_len;
}

int
ent_lock_for_select (
    struct ent_lock * p,
    struct ent_table * table,
    char const * column_name,
    size_t width)
{
	if (!p)
	{
		return -1;
	}

	struct ent_array * array = ent_table_column (
	                               table,
	                               column_name,
	                               width);

	if (!array)
	{
		return -1;
	}

	size_t columns_len = ent_array_len (p->columns);

	if (ent_array_set_len (p->columns, columns_len + 1) == -1)
	{
		return -1;
	}

	struct column_info * columns = ent_array_get (p->columns);

	columns[columns_len] = (struct column_info) { .width = width };

	size_t name_len = strlen (column_name) + 1;
	columns[columns_len].name = NULL;

	if (ent_alloc ((void**)&columns[columns_len].name , name_len) == -1)
	{
		return -1;
	}

	memcpy (columns[columns_len].name, column_name, name_len);

	return (int) columns_len;
}

size_t
ent_lock_tables_len (
    struct ent_lock const * lock)
{
	size_t len = 0;

	if (lock)
	{
		len = ent_array_len (lock->tables);
	}

	return len;
}

struct ent_table *
ent_lock_table (
    struct ent_lock const * lock,
    int table_id)
{
	if (!lock)
	{
		return NULL;
	}

	size_t index = (size_t) table_id;
	size_t tables_len = ent_array_len (lock->tables);

	if (tables_len <= index)
	{
		return NULL;
	}

	struct ent_table ** tables = ent_array_get (lock->tables);
	return tables[index];
}

struct column_info
ent_lock_column (
    struct ent_lock const * lock,
    int column_id)
{
	if (!lock)
	{
		return (struct column_info) {0};
	}

	size_t index = (size_t) column_id;
	size_t columns_len = ent_array_len (lock->columns);
	struct column_info * columns = ent_array_get (lock->columns);

	if (columns_len <= index)
	{
		return (struct column_info) {0};
	}

	return columns[index];
}
