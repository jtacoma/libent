#include "ent.h"
#include "table.h"
#include "rlist.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>

struct column_info
{
	char * name;
	struct ent_array * array;
};

struct ent_table
{
	size_t len;
	struct ent_array * columns;
	int refcount;
};

struct ent_table *
ent_table_alloc (
    size_t len)
{
	struct ent_table * t = malloc (sizeof (*t));

	if (t)
	{
		*t = (struct ent_table) {0};
		t->len = len;
		t->columns =  ent_array_alloc (sizeof (struct column_info));

		if (!t->columns)
		{
			free (t);
		}

		t->refcount += 1;
	}

	return t;
}

void
ent_table_incref (
    struct ent_table *t)
{
	t->refcount += 1;
}

void
ent_table_decref (
    struct ent_table * t)
{
	if (t && --t->refcount < 0)
	{
		size_t columns_len = ent_array_len (t->columns);
		struct column_info * columns = ent_array_ref (t->columns);

		if (columns_len)
		{
			for (size_t i = 0; i < columns_len; ++i)
			{
				free (columns[i].name);
				ent_array_free (columns[i].array);
			}

			ent_array_free (t->columns);
		}

		free (t);
	}
}

size_t
ent_table_columns_len (
    struct ent_table const * t)
{
	return ent_array_len (t->columns);
}

char const *
ent_table_column_info (
    struct ent_table const * t,
    size_t column_index,
    size_t * width)
{
	if (column_index >= ent_array_len (t->columns))
	{
		*width = 0;
		return NULL;
	}
	struct column_info * columns = ent_array_ref (t->columns);
	*width = ent_array_width (columns[column_index].array);
	return columns[column_index].name;
}

size_t
ent_table_len (
    struct ent_table const * table)
{
	if (!table)
	{
		return 0;
	}

	return table->len;
}

bool
ent_table_has_column_name (
    struct ent_table * t,
    char const * name)
{
	if (! (t && name))
	{
		return false;
	}

	size_t columns_len = ent_array_len (t->columns);
	struct column_info * columns = ent_array_ref (t->columns);
	for (size_t i = 0; i < columns_len; ++i)
	{
		if (strcmp (columns[i].name, name) == 0)
		{
			return true;
		}
	}

	return false;
}

struct ent_array *
ent_table_column (
    struct ent_table * t,
    char const * name,
    size_t width)
{
	if (! (t && name && width))
	{
		return NULL;
	}

	size_t columns_len = ent_array_len (t->columns);
	struct column_info * columns = ent_array_ref (t->columns);

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

	if (ent_array_set_len (t->columns, columns_len + 1) == -1)
	{
		return NULL;
	}

	columns = ent_array_ref (t->columns);

	size_t name_size = strlen (name) + 1;
	columns[columns_len].name = malloc (name_size);

	if (!columns[columns_len].name)
	{
		ent_array_set_len (t->columns, columns_len);
		// t->columns: is bigger and zero-padded, no problem
		// t->column_names: is bigger and zero-padded, no problem
		return NULL; // out of memory
	}

	memcpy (columns[columns_len].name, name, name_size);

	columns[columns_len].array = ent_array_alloc (width);

	if (!columns[columns_len].array)
	{
		// t->columns: is bigger and zero-padded, no problem
		// t->column_names: is bigger and zero-padded, no problem
		free (columns[columns_len].name);
		ent_array_set_len (t->columns, columns_len);
		return NULL; // out of memory
	}

	if (t->len && ent_array_set_len (columns[columns_len].array, t->len) == -1)
	{
		ent_array_free (columns[columns_len].array);
		free (columns[columns_len].name);
		return NULL; // out of memory
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
		return -1;
	}

	struct ent_rlist * keep = ent_rlist_alloc();
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
		ent_rlist_append (keep, begin, end);
	}

	size_t columns_len = ent_array_len (t->columns);

	struct column_info * src_columns = ent_array_ref (t->columns);

	struct ent_array * new_columns =
	    ent_array_alloc (sizeof (struct column_info));

	ent_array_set_len (new_columns, columns_len);

	struct column_info * dst_columns = ent_array_ref (new_columns);

	size_t new_len = t->len - ent_rlist_len (rlist);

	for (size_t i = 0; i < columns_len; ++i)
	{
		dst_columns[i].array =
		    ent_array_alloc (ent_array_width (src_columns[i].array));

		ent_array_set_len (dst_columns[i].array, new_len);

		if (!dst_columns[i].array)
		{
			for (size_t k = 0; k < i; ++k)
			{
				ent_array_free (dst_columns[i].array);
			}

			ent_array_free (new_columns);
			ent_rlist_free (keep);
			return -1;
		}
	}

	for (size_t i = 0; i < columns_len; ++i)
	{
		void * dst = ent_array_ref (dst_columns[i].array);
		void const * src = ent_array_get (src_columns[i].array);
		size_t width = ent_array_width (dst_columns[i].array);

		if (ent_rlist_select (keep, dst, src, width) == -1)
		{
			for (size_t k = 0; k < i; ++k)
			{
				ent_array_free (dst_columns[i].array);
			}

			ent_array_free (new_columns);
			ent_rlist_free (keep);
			return -1;
		}
	}

	ent_rlist_free (keep);

	for (size_t i = 0; i < columns_len; ++i)
	{
		dst_columns[i].name = src_columns[i].name;
		src_columns[i].name = NULL;
		ent_array_free (src_columns[i].array);
	}

	ent_array_free (t->columns);
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
		return -1;
	}

	t->len += add;

	size_t columns_len = ent_array_len (t->columns);
	struct column_info * columns = ent_array_ref (t->columns);
	for (size_t i = 0; i < columns_len; ++i)
	{
		if (ent_array_set_len (columns[i].array, t->len) == -1)
		{
			return -1;
		}
	}

	return 0;
}
