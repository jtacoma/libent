#include <assert.h>
#include <stdint.h> // for "array.h"
#include <string.h> // strlen, memcpy

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "processor.h"
#include "table.h"

struct ent_processor
{
	struct ent_array * tables; // struct ent_table **
	struct ent_array * columns; // struct column_info *
	ent_procfunc func;
	void * func_arg;
};

struct ent_processor *
ent_processor_alloc()
{
	struct ent_processor * p = NULL;

	if (ent_alloc ((void**)&p, sizeof (*p)) == 0)
	{
		*p = (struct ent_processor) {0};
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
ent_processor_free (
    struct ent_processor * p)
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
ent_processor_use_table (
    struct ent_processor * p,
    struct ent_table * table,
    char const * mode)
{
	if (! (p && table && mode))
	{
		return -1;
	}

	ent_table_incref (table);
	//TODO: stop ignoring mode

	size_t tables_len = ent_array_len (p->tables);

	if (ent_array_set_len (p->tables, tables_len + 1) == -1)
	{
		return -1;
	}

	struct ent_table ** tables = ent_array_get (p->tables);

	assert (tables);

	tables[tables_len] = table;

	return (int) tables_len;
}

int
ent_processor_use_column (
    struct ent_processor * p,
    struct ent_table * table,
    char const * column_name,
    size_t width,
    char const * mode)
{
	if (!p)
	{
		return -1;
	}

	(void)mode;//TODO: stop ignoring mode

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
ent_processor_set_function (
    struct ent_processor * p,
    ent_procfunc function,
    void * arg)
{
	if (! (p && function))
	{
		return -1;
	}

	p->func = function;
	p->func_arg = arg;
	return 0;
}

size_t
ent_processor_tables_len (
    struct ent_processor const * processor)
{
	size_t len = 0;

	if (processor)
	{
		len = ent_array_len (processor->tables);
	}

	return len;
}

struct ent_table *
ent_processor_table (
    struct ent_processor const * processor,
    int table_id)
{
	if (!processor)
	{
		return NULL;
	}

	size_t index = (size_t) table_id;
	size_t tables_len = ent_array_len (processor->tables);

	if (tables_len <= index)
	{
		return NULL;
	}

	struct ent_table ** tables = ent_array_get (processor->tables);
	return tables[index];
}

struct column_info
ent_processor_column (
    struct ent_processor const * processor,
    int column_id)
{
	if (!processor)
	{
		return (struct column_info) {0};
	}

	size_t index = (size_t) column_id;
	size_t columns_len = ent_array_len (processor->columns);
	struct column_info * columns = ent_array_get (processor->columns);

	if (columns_len <= index)
	{
		return (struct column_info) {0};
	}

	return columns[index];
}
