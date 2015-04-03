#include "ent.h"
#include "model.h"
#include "array.h"
#include "table.h"
#include "processor.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct ent_processor
{
	struct ent_model * model;
	struct ent_array * tables; // struct ent_table **
	struct ent_array * columns; // struct column_info *
	ent_procfunc func;
	void * func_arg;
};

struct ent_processor *
ent_processor_alloc (
    struct ent_model * m)
{
	if (!m)
	{
		return NULL;
	}

	struct ent_processor * p = malloc (sizeof (*p));

	if (p)
	{
		*p = (struct ent_processor) {.model = m};
		p->tables = ent_array_alloc (sizeof (struct ent_table *));
		p->columns = ent_array_alloc (sizeof (struct column_info));
	}

	return p;
}

struct ent_processor *
ent_processor_cpy_alloc (
    struct ent_processor const * src)
{
	struct ent_processor * dst = ent_processor_alloc (src->model);

	if (dst)
	{
		*dst = *src;
		dst->tables = ent_array_cpy_alloc (dst->tables);

		if (!dst->tables)
		{
			free (dst);
			return NULL;
		}

		dst->columns = ent_array_cpy_alloc (dst->columns);

		if (!dst->columns)
		{
			ent_array_free (dst->tables);
			free (dst);
			return NULL;
		}
	}

	return dst;
}

void
ent_processor_free (
    struct ent_processor * p)
{
	if (p)
	{
		size_t tables_len = ent_array_len (p->tables);
		struct ent_table ** tables = ent_array_ref (p->tables);

		for (size_t i = 0; i < tables_len; ++i)
		{
			ent_table_decref (tables[i]);
		}

		size_t columns_len = ent_array_len (p->columns);
		struct column_info * columns = ent_array_ref (p->columns);

		for (size_t i = 0; i < columns_len; ++i)
		{
			free (columns[i].name);
		}

		ent_array_free (p->tables);
		ent_array_free (p->columns);
		free (p);
	}
}

int
ent_processor_use_table (
    struct ent_processor * p,
    char const * table_name,
    char const * mode)
{
	if (!p)
	{
		return -1;
	}

	(void)mode;//TODO: stop ignoring mode

	size_t tables_len = ent_array_len (p->tables);

	if (ent_array_set_len (p->tables, tables_len + 1) == -1)
	{
		return -1;
	}

	struct ent_table ** tables = ent_array_ref (p->tables);

	if (!tables)
	{
		return -1;
	}

	tables[tables_len] = ent_model_get (p->model, table_name);

	if (!tables[tables_len])
	{
		ent_array_set_len (p->tables, tables_len);
		return -1;
	}

	return (int) tables_len;
}

int
ent_processor_use_column (
    struct ent_processor * p,
    int table_id,
    char const * column_name,
    size_t width,
    char const * mode)
{
	if (!p)
	{
		return -1;
	}

	(void)mode;//TODO: stop ignoring mode

	size_t index = (size_t) table_id;
	size_t tables_len = ent_array_len (p->tables);

	if (tables_len <= index)
	{
		return -1;
	}

	struct ent_table ** tables = ent_array_ref (p->tables);

	struct ent_table * table = tables[index];

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

	struct column_info * columns = ent_array_ref (p->columns);

	columns[columns_len] = (struct column_info) { .width = width };

	size_t name_len = strlen (column_name) + 1;
	columns[columns_len].name = malloc (name_len);
	if (!columns[columns_len].name)
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

	struct ent_table ** tables = ent_array_ref (processor->tables);
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
	struct column_info * columns = ent_array_ref (processor->columns);

	if (columns_len <= index)
	{
		return (struct column_info) {0};
	}

	return columns[index];
}
