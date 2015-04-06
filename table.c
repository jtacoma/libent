#include "ent.h"
#include "table.h"
#include "rlist.h"
#include "array.h"
#include "alloc.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
    struct ent_table * t,
    struct ent_rlist const * rlist)
{
	if (! (t && rlist))
	{
		errno = EINVAL;
		return -1;
	}

	struct ent_rlist * keep = ent_rlist_alloc();

	if (!keep)
	{
		return -1;
	}

	size_t del_ranges_len = 0;
	struct ent_rlist_range const * del_ranges =
	    ent_rlist_ranges (rlist, &del_ranges_len);

	for (size_t i = 0; i < del_ranges_len; ++i)
	{
		if (del_ranges[i].begin == 0)
		{
			continue;
		}

		size_t begin = i == 0 ? 0 : del_ranges[i - 1].end;
		size_t end = del_ranges[i].begin;

		if (ent_rlist_append (keep, begin, end) == -1)
		{
			ent_rlist_free (keep);
			return -1;
		}
	}

	if (del_ranges_len && del_ranges[del_ranges_len - 1].end < t->len)
	{
		size_t begin = del_ranges[del_ranges_len - 1].end;

		if (ent_rlist_append (keep, begin, t->len) == -1)
		{
			ent_rlist_free (keep);
			return -1;
		}
	}

	size_t columns_len = ent_column_array_len (t->columns);

	struct column * src_columns = ent_column_array_get (t->columns);

	struct ent_column_array * new_columns = ent_column_array_alloc();

	if (!new_columns)
	{
		ent_rlist_free (keep);
		return -1;
	}

	if (ent_column_array_set_len (new_columns, columns_len) == -1)
	{
		ent_rlist_free (keep);
		ent_column_array_free (new_columns);
		return -1;
	}

	struct column * dst_columns = ent_column_array_get (new_columns);

	size_t new_len = t->len - ent_rlist_len (rlist);

	for (size_t i = 0; i < columns_len; ++i)
	{
		dst_columns[i].array =
		    ent_array_alloc (ent_array_width (src_columns[i].array));
		if (!dst_columns[i].array ||
		        ent_array_set_len (dst_columns[i].array, new_len) == -1)
		{
			if (dst_columns[i].array)
			{
				i += 1;
			}

			for (size_t k = 0; k < i; ++k)
			{
				ent_array_free (dst_columns[k].array);
			}

			ent_column_array_free (new_columns);
			ent_rlist_free (keep);
			return -1;
		}
	}

	for (size_t i = 0; new_len && i < columns_len; ++i)
	{
		void * dst = ent_array_get (dst_columns[i].array);
		void const * src = ent_array_get_const (src_columns[i].array);
		size_t width = ent_array_width (dst_columns[i].array);

		assert (ent_rlist_select (keep, dst, src, width) == 0);
	}

	ent_rlist_free (keep);

	for (size_t i = 0; i < columns_len; ++i)
	{
		dst_columns[i].name = src_columns[i].name;
		src_columns[i].name = NULL;
		ent_array_free (src_columns[i].array);
	}

	ent_column_array_free (t->columns);
	t->columns = new_columns;
	t->len = new_len;
	return 0;
}

int
ent_table_grow (
    struct ent_table * t,
    size_t add)
{
	if (! (t && add > 0))
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
		if (ent_array_set_len (columns[i].array, new_len) == -1)
		{
			return -1;
		}
	}

	t->len = new_len;

	return 0;
}

int
ent_table_pre_grow (
    struct ent_table * t,
    size_t add)
{
	if (! t)
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
