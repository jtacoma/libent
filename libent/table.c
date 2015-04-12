#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "rlist.h"
#include "table.h"

struct column
{
	char * name;
	struct ent_array * array;
};

ent_array_typed (struct column, column);

struct ent_table
{
	size_t len;
	struct ent_column_array * columns;
	int refcount;
};

struct ent_table *
ent_table_alloc (void)
{
	struct ent_table * t = NULL;

	if (ent_alloc ((void**)&t, sizeof (*t)) == 0)
	{
		*t = (struct ent_table) {0};
		t->refcount = 1;
		t->columns =  ent_column_array_alloc();

		if (!t->columns)
		{
			ent_alloc ((void**)&t, 0);
			return NULL;
		}
	}

	return t;
}

void
ent_table_incref (
    struct ent_table *t)
{
	if (!t)
	{
		errno = EINVAL;
		return;
	}

	t->refcount += 1;
}

void
ent_table_free (
    struct ent_table * t)
{
	if (!t)
	{
		errno = EINVAL;
		return;
	}

	assert (--t->refcount >= 0);

	if (!t->refcount)
	{
		if (t->columns)
		{
			size_t columns_len = ent_column_array_len (t->columns);
			struct column * columns = ent_column_array_get (t->columns);

			for (size_t i = 0; i < columns_len; ++i)
			{
				ent_alloc ((void**)&columns[i].name, 0);
				ent_array_free (columns[i].array);
			}

			ent_column_array_free (t->columns);
		}

		ent_alloc ((void**)&t, 0);
	}
}

size_t
ent_table_columns_len (
    struct ent_table const * t)
{
	if (!t)
	{
		errno = EINVAL;
		return 0;
	}

	return ent_column_array_len (t->columns);
}

char const *
ent_table_column_info (
    struct ent_table const * t,
    size_t column_index,
    size_t * width)
{
	if (! (t && width))
	{
		errno = EINVAL;
		return NULL;
	}

	if (column_index >= ent_column_array_len (t->columns))
	{
		errno = EINVAL;
		*width = 0;
		return NULL;
	}

	struct column * columns = ent_column_array_get (t->columns);
	*width = ent_array_width (columns[column_index].array);
	return columns[column_index].name;
}

size_t
ent_table_len (
    struct ent_table const * table)
{
	if (!table)
	{
		errno = EINVAL;
		return 0;
	}

	return table->len;
}

struct ent_array *
ent_table_column (
    struct ent_table * t,
    char const * name,
    size_t width)
{
	if (! (t && name && width))
	{
		errno = EINVAL;
		return NULL;
	}

	size_t columns_len = ent_column_array_len (t->columns);
	struct column * columns = ent_column_array_get (t->columns);

	for (size_t i = 0; i < columns_len; ++i)
	{
		if (strcmp (columns[i].name, name) == 0)
		{
			if (ent_array_width (columns[i].array) == width)
			{
				return columns[i].array;
			}
			else
			{
				return NULL;
			}
		}
	}

	if (ent_column_array_set_len (t->columns, columns_len + 1) == -1)
	{
		return NULL;
	}

	columns = ent_column_array_get (t->columns);

	size_t name_size = strlen (name) + 1;
	if (ent_alloc ((void**)& columns[columns_len].name, name_size) == -1)
	{
		ent_column_array_set_len (t->columns, columns_len);
		return NULL;
	}

	memcpy (columns[columns_len].name, name, name_size);

	columns[columns_len].array = ent_array_alloc (width);

	if (!columns[columns_len].array)
	{
		ent_alloc ((void**)&columns[columns_len].name, 0);
		ent_column_array_set_len (t->columns, columns_len);
		return NULL;
	}

	if (t->len && ent_array_set_len (columns[columns_len].array, t->len) == -1)
	{
		ent_alloc ((void**)&columns[columns_len].name, 0);
		ent_array_free (columns[columns_len].array);
		ent_column_array_set_len (t->columns, columns_len);
		return NULL;
	}

	return columns[columns_len].array;
}

int
ent_table_delete (
    struct ent_table * table,
    struct ent_rlist const * keep)
{
	if (! (table && keep))
	{
		errno = EINVAL;
		return -1;
	}

	size_t ranges_len;
	struct ent_rlist_range const * ranges = ent_rlist_ranges (keep, &ranges_len);

	if (ranges_len && ranges[ranges_len - 1].end > table->len)
	{
		errno = EINVAL;
		return -1;
	}

	size_t columns_len = ent_column_array_len (table->columns);
	struct column * columns = ent_column_array_get (table->columns);

	size_t new_len = ent_rlist_len (keep);

	for (size_t i = 0; i < columns_len; ++i)
	{
		assert (ent_array_len (columns[i].array) == table->len);
		assert (ent_array_select_in_place (columns[i].array, keep) == 0);
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		assert (ent_array_set_len (columns[i].array, new_len) == 0);
	}

	table->len = new_len;
	return 0;
}

int
ent_table_grow (
    struct ent_table * t,
    size_t add)
{
	if (!t)
	{
		errno = EINVAL;
		return -1;
	}

	if (ent_table_pre_grow (t, add) == -1)
	{
		return -1;
	}

	size_t new_len = t->len + add;
	size_t columns_len = ent_column_array_len (t->columns);
	struct column * columns = ent_column_array_get (t->columns);

	for (size_t i = 0; i < columns_len; ++i)
	{
		// Due to ent_table_pre_grow above already taking care of
		// allocations, there's no reason for this to fail.
		assert (ent_array_set_len (columns[i].array, new_len) == 0);
	}

	t->len = new_len;

	return 0;
}

int
ent_table_pre_grow (
    struct ent_table * t,
    size_t add)
{
	if (!t)
	{
		errno = EINVAL;
		return -1;
	}

	if (!add)
	{
		return 0;
	}

	size_t new_len = t->len + add;

	size_t columns_len = ent_column_array_len (t->columns);
	struct column * columns = ent_column_array_get (t->columns);

	for (size_t i = 0; i < columns_len; ++i)
	{
		if (ent_array_preallocate (columns[i].array, new_len) == -1)
		{
			return -1;
		}
	}

	return 0;
}

int
ent_table_insert (
    struct ent_table * dst_table,
    struct ent_table * src_table)
{
	size_t start = ent_table_len (dst_table);

	if (ent_table_grow (dst_table, ent_table_len (src_table)) == -1)
	{
		return -1;
	}

	size_t columns_len = ent_table_columns_len (src_table);

	for (size_t k = 0; k < columns_len; ++k)
	{
		size_t width;

		char const * name =
		    ent_table_column_info (src_table, k, &width);

		struct ent_array * src_array =
		    ent_table_column (src_table, name, width);

		struct ent_array * dst_array =
		    ent_table_column (dst_table, name, width);

		if (dst_array == NULL)
		{
			return -1;
		}

		uint8_t * dst = ent_array_get (dst_array);

		dst += width * start;

		uint8_t const * src = ent_array_get_const (src_array);

		memcpy (dst, src, width * ent_table_len (src_table));
	}

	return 0;
}
