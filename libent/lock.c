#include <assert.h>
#include <stdint.h> // for "array.h"
#include <string.h> // strlen, memcpy

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "column.h"
#include "table.h"
#include "lock.h"

struct ent_lock
{
	struct ent_array * tables; // struct ent_table **
	struct ent_array * columns; // struct ent_column *
	ent_procfunc func;
	void * func_arg;
};

struct ent_lock *
ent_lock_alloc ()
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
		p->columns = ent_array_alloc (sizeof (struct ent_column));
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
    struct ent_column const * column)
{
	if (!p)
	{
		return -1;
	}

	struct ent_array * array = ent_table_column (
	                               table,
	                               column);

	if (!array)
	{
		return -1;
	}

	size_t columns_len = ent_array_len (p->columns);

	if (ent_array_set_len (p->columns, columns_len + 1) == -1)
	{
		return -1;
	}

	struct ent_column * columns = ent_array_get (p->columns);

	columns[columns_len] = *column;

	return (int) columns_len;
}

int
ent_lock_for_select (
    struct ent_lock * p,
    struct ent_table * table,
    struct ent_column const * column)
{
	if (!p)
	{
		return -1;
	}

	struct ent_array * array = ent_table_column (
	                               table,
	                               column);

	if (!array)
	{
		return -1;
	}

	size_t columns_len = ent_array_len (p->columns);

	if (ent_array_set_len (p->columns, columns_len + 1) == -1)
	{
		return -1;
	}

	struct ent_column * columns = ent_array_get (p->columns);

	columns[columns_len] = *column;

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
