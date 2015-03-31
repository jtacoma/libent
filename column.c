#include "ent.h"
#include "column.h"
#include "rlist.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	void * start;
	size_t width;
	size_t len;
};

struct ent_column *
ent_column_alloc (
    size_t width)
{
	if (! (width))
	{
		return NULL;
	}

	struct ent_column * column = malloc (sizeof (*column));

	if (!column)
	{
		return NULL; // out of memory
	}

	*column = (struct ent_column) {.width = width };

	return column;
}

void ent_column_free (struct ent_column * c)
{
	if (c)
	{
		if (c->start)
		{
			free (c->start);
		}

		free (c);
	}
}

void const * ent_column_get (struct ent_column const * c)
{
	if (!c)
	{
		return NULL;
	}

	return c->start;
}

size_t ent_column_len (struct ent_column const * c)
{
	if (!c)
	{
		return 0;
	}

	return c->len;
}

size_t ent_column_width (struct ent_column const * c)
{
	if (!c)
	{
		return 0;
	}

	return c->width;
}

int ent_column_set_len (struct ent_column * c, size_t len)
{
	if (!c)
	{
		return -1;
	}

	if (len > 0)
	{
		void * mem = realloc (c->start, c->width * len);

		if (!mem)
		{
			return -1; // out of memory
		}

		if (len > c->len)
		{
			size_t added = len - c->len;
			memset (
			    ((uint8_t*)mem) + (c->width * c->len),
			    0,
			    c->width * added);
		}

		c->start = mem;
	}
	else if (c->start)
	{
		free (c->start);
		c->start = NULL;
	}

	c->len = len;
	return 0;
}

void * ent_column_ref (struct ent_column * c)
{
	if (!c)
	{
		return NULL;
	}

	return c->start;
}

int ent_column_select (struct ent_column * dst,
                       struct ent_column const * src,
                       struct ent_rlist const * rlist)
{
	if (! (dst && src && rlist && src->width == dst->width))
	{
		return -1;
	}

	size_t width = src->width;

	size_t dst_cap = ent_column_len (dst);
	uint8_t * dst_ptr = ent_column_ref (dst);
	uint8_t * dst_next = dst_ptr;

	size_t src_len = ent_column_len (src);
	uint8_t const * src_ptr = ent_column_get (src);

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
