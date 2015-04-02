#include "ent.h"
#include "array.h"
#include "rlist.h"

#include <stdlib.h>
#include <string.h>

struct ent_array
{
	void * start;
	size_t width;
	size_t len;
	size_t cap;
};

struct ent_array *
ent_array_alloc (
    size_t width)
{
	if (! (width))
	{
		return NULL;
	}

	struct ent_array * array = malloc (sizeof (*array));

	if (array)
	{
		*array = (struct ent_array) {.width = width };
	}

	return array;
}

struct ent_array *
ent_array_cpy_alloc (
    struct ent_array const * src)
{
	if (!src)
	{
		return NULL;
	}

	struct ent_array * dst = ent_array_alloc (src->width);

	if (ent_array_set_len (dst, src->len) == -1)
	{
		free (dst);
		return NULL;
	}

	memcpy (dst->start, src->start, src->width * src->len);

	return dst;
}

void
ent_array_free (
    struct ent_array * a)
{
	if (a)
	{
		if (a->start)
		{
			free (a->start);
		}

		free (a);
	}
}

void const *
ent_array_get (
    struct ent_array const * a)
{
	if (!a)
	{
		return NULL;
	}

	return a->start;
}

size_t
ent_array_len (
    struct ent_array const * a)
{
	if (!a)
	{
		return 0;
	}

	return a->len;
}

size_t
ent_array_width (
    struct ent_array const * a)
{
	if (!a)
	{
		return 0;
	}

	return a->width;
}

int ent_array_set_len (struct ent_array * a, size_t len)
{
	if (!a)
	{
		return -1;
	}

	if (len > a->cap)
	{
		size_t cap = a->cap
		             ? a->cap / 2 * 3 + 1
		             : 64 / a->width;

		if (len > cap)
		{
			cap = len;
		}

		void * mem = realloc (a->start, a->width * cap);

		if (!mem)
		{
			return -1; // out of memory
		}

		size_t added = len - a->len;
		memset (
		    ((uint8_t*)mem) + (a->width * a->len),
		    0,
		    a->width * added);

		a->start = mem;
		a->cap = cap;
	}
	else if (!len && a->start)
	{
		free (a->start);
		a->start = NULL;
		a->cap = 0;
	}

	a->len = len;
	return 0;
}

void *
ent_array_ref (
    struct ent_array * a)
{
	if (!a)
	{
		return NULL;
	}

	return a->start;
}

int
ent_array_select (
    struct ent_array * dst,
    struct ent_array const * src,
    struct ent_rlist const * rlist)
{
	if (! (dst && src && rlist && src->width == dst->width))
	{
		return -1;
	}

	size_t width = src->width;

	size_t dst_cap = ent_array_len (dst);
	uint8_t * dst_ptr = ent_array_ref (dst);
	uint8_t * dst_next = dst_ptr;

	size_t src_len = ent_array_len (src);
	uint8_t const * src_ptr = ent_array_get (src);

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
