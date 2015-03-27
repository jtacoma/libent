#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	void * start;
	struct ent_typeinfo type;
	size_t len;
};

struct ent_column *
ent_column_alloc (char const * type, size_t len)
{
	struct ent_column * column =
	    ent_realloc_carray (NULL, 1, sizeof (*column), true);

	if (column == NULL)
	{
		return NULL; // out of memory
	}

	if (ent_typeinfo_parse (&column->type, type) == -1)
	{
		free (column); // out of memory
		return NULL; // out of memory
	}

	if (len > 0)
	{
		size_t width = ent_typeinfo_width (&column->type);
		column->start = ent_realloc_carray (NULL, len, width, true);

		if (column->start == NULL)
		{
			ent_column_free (column); // out of memory
			return NULL; // out of memory
		}

		column->len = len;
	}

	return column;
}

void ent_column_free (struct ent_column * c)
{
	if (c->start != NULL)
	{
		ent_realloc_free (c->start);
	}
	ent_realloc_free (c);
}

void const * ent_column_get (struct ent_column const * c, size_t * len)
{
	*len = c->len;
	return c->start;
}

struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c)
{
	return &c->type;
}

char const * ent_column_typename (struct ent_column const * c)
{
	return ent_typeinfo_name (&c->type);
}

void * ent_column_ref (struct ent_column * c, size_t *len)
{
	*len = c->len;
	return c->start;
}

int ent_column_select (struct ent_column * dst,
                       struct ent_column const * src,
                       struct ent_rlist const * rlist)
{
	struct ent_typeinfo const * dst_type = ent_column_typeinfo (dst);
	struct ent_typeinfo const * src_type = ent_column_typeinfo (src);
	if (!ent_typeinfo_equal (dst_type, src_type))
	{
		return -1; // mismatched types
	}
	size_t width = ent_typeinfo_width (dst_type);

	size_t dst_cap = 0;
	uint8_t * dst_ptr = ent_column_ref (dst, &dst_cap);
	uint8_t * dst_next = dst_ptr;

	size_t src_len = 0;
	uint8_t const * src_ptr = ent_column_get (src, &src_len);

	size_t ranges_len = 0;
	struct ent_rlist_range const * ranges =
	    ent_rlist_ranges (rlist, &ranges_len);

	size_t required_dst_len = ent_rlist_len (rlist);
	size_t required_src_len = ranges_len
	                          ? ranges[ranges_len - 1].end
	                          : 0;

	if (required_src_len > src_len)
	{
		return -1;
	}

	if (required_dst_len != dst_cap)
	{
		// Sometime this case might become a "replace" or "append"
		// operation, but for now it's just not acceptable.
		return -1;
	}

	for (size_t i = 0; i < ranges_len; ++i)
	{
		size_t n = (ranges[i].end - ranges[i].begin) * width;
		uint8_t const * src_range = src_ptr + ranges[i].begin * width;
		memcpy (dst_next, src_range, n);
		dst_next += n;
	}

	return 0;
}
