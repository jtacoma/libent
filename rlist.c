#include "ent.h"
#include "ent_internal.h"

#include <string.h>
#include <stdlib.h>

struct ent_rlist
{
	size_t vlen;
	size_t rlen;
	struct ent_rlist_range * ranges;
};

struct ent_rlist * ent_rlist_alloc ()
{
	return calloc(1, sizeof(struct ent_rlist));
}

void ent_rlist_free (struct ent_rlist * rlist)
{
	free(rlist->ranges);
	free(rlist);
}

size_t ent_rlist_len (struct ent_rlist const * rlist)
{
	return rlist->vlen;
}

struct ent_rlist_range const * ent_rlist_ranges(
    struct ent_rlist const * rlist, size_t *len)
{
	*len = rlist->rlen;
	return rlist->ranges;
}

int ent_rlist_append (struct ent_rlist * rlist, size_t begin, size_t end)
{
	if (end <= begin ||
	        (rlist->ranges != NULL &&
	         begin < rlist->ranges[rlist->rlen - 1].begin))
	{
		return -1; // invalid begin,end values
	}

	if (rlist->ranges != NULL &&
	        end <= rlist->ranges[rlist->rlen - 1].end)
	{
		return 0; // no-op
	}

	if (rlist->ranges != NULL &&
	        begin <= rlist->ranges[rlist->rlen - 1].end)
	{
		rlist->vlen += end - rlist->ranges[rlist->rlen - 1].end;
		rlist->ranges[rlist->rlen - 1].end = end;
	}
	else
	{
		struct ent_rlist_range * ranges = realloc(
		                                      rlist->ranges,
		                                      sizeof(*ranges) * (rlist->rlen + 1));
		if (ranges == NULL)
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

/* This code is not being used right now but I put a lot of thought into it
 * and I expect it will come in handy as soon as I want to start combining
 * rlists.
int ent_rlist_delete (struct ent_rlist * rlist, struct ent_rlist const * src)
{
	struct ent_rlist * collect = ent_rlist_alloc();
	if (collect == NULL)
	{
		return -1;
	}

	int rc = 0;
	struct ent_rlist_range * old = rlist->ranges;
	struct ent_rlist_range * old_end =
			    rlist->ranges == NULL ? NULL : rlist->ranges + rlist->rlen;
	struct ent_rlist_range * del = src->ranges;
	struct ent_rlist_range * del_end =
			    src->ranges == NULL ? NULL : src->ranges + src->rlen;

	do
{
		while (old != NULL && del != NULL)
		{
			while (del->end < old->begin)
			{
				if (++del == del_end)
				{
					del = NULL;
					break;
				}
			}
			while (del == NULL || old->end < del->begin)
			{
				rc = ent_rlist_append(collect, old->begin, old->end);
				if (rc == -1)
				{
					break;
				}
				if (++old == old_end)
				{
					old = NULL;
					break;
				}
			}
			if (old == NULL || del == NULL || rc == -1)
			{
				break;
			}
			if (old->begin < del->begin)
			{
				rc = ent_rlist_append(collect, old->begin, del->begin);
				if (rc == -1)
				{
					break;
				}
			}
			if (del->end < old->end)
			{
				rc = ent_rlist_append(collect, del->end, old->end);
				if (rc == -1)
				{
					break;
				}
			}
			if (++old == old_end)
			{
				break;
			}
		}
		if (rc == 0)
		{
			free(rlist->ranges);
			*rlist = *collect;
			free(collect);//Maybe collect can be on the stack instead?
		}
	}
	while (0);

	return rc;
}
*/
