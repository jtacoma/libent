#include "ent.h"
#include "alloc.h"
#include "rlist.h"
#include "array.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>

typedef struct ent_rlist_range range;
ent_array_typed (range);

struct ent_rlist
{
	size_t vlen;
	struct ent_range_array * ranges;
};

struct ent_rlist *
ent_rlist_alloc()
{
	struct ent_rlist * rlist = NULL;

	if (ent_alloc ((void**)&rlist, sizeof (*rlist)) == 0)
	{
		*rlist = (struct ent_rlist) {0};
		rlist->ranges = ent_range_array_alloc();

		if (rlist->ranges == NULL)
		{
			ent_alloc ((void**)&rlist, 0);
			return NULL;
		}
	}

	return rlist;
}

void
ent_rlist_free (
    struct ent_rlist * rlist)
{
	if (!rlist)
	{
		errno = EINVAL;
		return;
	}

	if (rlist->ranges)
	{
		ent_range_array_free (rlist->ranges);
		rlist->ranges = NULL;
	}

	ent_alloc ((void**)&rlist, 0);
}

size_t
ent_rlist_len (
    struct ent_rlist const * rlist)
{
	if (!rlist)
	{
		errno = EINVAL;
		return 0;
	}

	return rlist->vlen;
}

struct ent_rlist_range const *
ent_rlist_ranges (
    struct ent_rlist const * rlist,
    size_t *len)
{
	if (!rlist)
	{
		errno = EINVAL;
		return NULL;
	}

	*len = ent_range_array_len (rlist->ranges);
	return ent_range_array_get_const (rlist->ranges);
}

int
ent_rlist_append (
    struct ent_rlist * rlist,
    size_t begin,
    size_t end)
{
	if (!rlist || end <= begin)
	{
		errno = EINVAL;
		return -1;
	}

	size_t len = ent_range_array_len (rlist->ranges);
	struct ent_rlist_range * ranges = ent_range_array_get (rlist->ranges);

	if (ranges && begin < ranges[len - 1].begin)
	{
		errno = EINVAL;
		return -1;
	}

	if (ranges && end <= ranges[len - 1].end)
	{
		return 0; // no-op
	}

	if (ranges && begin <= ranges[len - 1].end)
	{
		rlist->vlen += end - ranges[len - 1].end;
		ranges[len - 1].end = end;
	}
	else
	{
		if (ent_range_array_set_len (rlist->ranges, len + 1) == -1)
		{
			return -1; // out of memory
		}

		ranges = ent_range_array_get (rlist->ranges);
		ranges[len].begin = begin;
		ranges[len].end = end;
		rlist->vlen += end - begin;
	}

	return 0;
}

int
ent_rlist_select (
    struct ent_rlist const * rlist,
    void * dst,
    void const * src,
    size_t width)
{
	if (! (rlist && dst && src && width))
	{
		errno = EINVAL;
		return -1;
	}

	uint8_t * dst_ptr = dst;
	uint8_t * dst_next = dst_ptr;
	uint8_t const * src_ptr = src;
	size_t ranges_len = 0;
	struct ent_rlist_range const * ranges =
	    ent_rlist_ranges (rlist, &ranges_len);

	for (size_t i = 0; i < ranges_len; ++i)
	{
		size_t n = (ranges[i].end - ranges[i].begin) * width;
		uint8_t const * src_range = src_ptr + ranges[i].begin * width;
		memcpy (dst_next, src_range, n);
		dst_next += n;
	}

	return 0;
}
