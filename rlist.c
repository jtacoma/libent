#include <errno.h>
#include <stdint.h> // for "array.h"
#include <string.h> // memcpy

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "rlist.h"

ent_array_typed (struct ent_rlist_range, range);

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
ent_rlist_append_inverse (
    struct ent_rlist * dst,
    struct ent_rlist const * src,
    size_t len)
{
	if (! (dst && src))
	{
		errno = EINVAL;
		return -1;
	}

	size_t src_ranges_len = 0;
	struct ent_rlist_range const * src_ranges =
	    ent_rlist_ranges (src, &src_ranges_len);

	for (size_t i = 0; i < src_ranges_len; ++i)
	{
		if (src_ranges[i].begin == 0)
		{
			continue;
		}

		size_t begin = i == 0 ? 0 : src_ranges[i - 1].end;
		size_t end = src_ranges[i].begin;

		if (ent_rlist_append (dst, begin, end) == -1)
		{
			return -1;
		}
	}

	if (src_ranges_len && src_ranges[src_ranges_len - 1].end < len)
	{
		size_t begin = src_ranges[src_ranges_len - 1].end;

		if (ent_rlist_append (dst, begin, len) == -1)
		{
			return -1;
		}
	}

	return 0;
}
