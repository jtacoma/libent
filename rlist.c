#include "ent.h"
#include "rlist.h"

#include <string.h>
#include <stdlib.h>

struct ent_rlist
{
	size_t vlen;
	size_t rlen;
	struct ent_rlist_range * ranges;
};

struct ent_rlist * ent_rlist_alloc()
{
	return calloc (1, sizeof (struct ent_rlist));
}

void
ent_rlist_free (struct ent_rlist * rlist)
{
	if (rlist)
	{
		if (rlist->ranges)
		{
			free (rlist->ranges);
			rlist->ranges = NULL;
		}

		free (rlist);
	}
}

size_t
ent_rlist_len (struct ent_rlist const * rlist)
{
	if (!rlist)
	{
		return 0;
	}

	return rlist->vlen;
}

struct ent_rlist_range const *
ent_rlist_ranges (struct ent_rlist const * rlist,
                  size_t *len)
{
	if (!rlist)
	{
		return NULL;
	}

	*len = rlist->rlen;
	return rlist->ranges;
}

int
ent_rlist_append (struct ent_rlist * rlist,
                  size_t begin,
                  size_t end)
{
	if (!rlist || end <= begin ||
	        (rlist->ranges != NULL &&
	         begin < rlist->ranges[rlist->rlen - 1].begin))
	{
		return -1; // null rlist or invalid begin,end values
	}

	if (rlist->ranges &&
	        end <= rlist->ranges[rlist->rlen - 1].end)
	{
		return 0; // no-op
	}

	if (rlist->ranges &&
	        begin <= rlist->ranges[rlist->rlen - 1].end)
	{
		rlist->vlen += end - rlist->ranges[rlist->rlen - 1].end;
		rlist->ranges[rlist->rlen - 1].end = end;
	}
	else
	{
		struct ent_rlist_range * ranges = realloc (rlist->ranges, sizeof (*ranges) * (rlist->rlen + 1));

		if (!ranges)
		{
			return -1; // out of memory
		}

		ranges[rlist->rlen].begin = begin;
		ranges[rlist->rlen].end = end;
		rlist->ranges = ranges;
		rlist->rlen += 1;
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
