#include "ent.h"
#include "ent_internal.h"

#include <string.h>
#include <stdlib.h>

struct ent_range
{
	size_t vlen;
	size_t rlen;
	struct ent_range_chunk * chunks;
};

struct ent_range * ent_range_alloc ()
{
	return calloc(1, sizeof(struct ent_range));
}

void ent_range_free (struct ent_range * range)
{
	free(range->chunks);
	free(range);
}

size_t ent_range_len (struct ent_range const * range)
{
	return range->vlen;
}

struct ent_range_chunk const * ent_range_chunks(
    struct ent_range const * range, size_t *len)
{
	*len = range->rlen;
	return range->chunks;
}

int ent_range_append (struct ent_range * range, size_t begin, size_t end)
{
	if (end <= begin ||
	        (range->chunks != NULL &&
	         begin < range->chunks[range->rlen - 1].begin))
	{
		return -1; // invalid begin,end values
	}

	if (range->chunks != NULL &&
	        end <= range->chunks[range->rlen - 1].end)
	{
		return 0; // no-op
	}

	if (range->chunks != NULL &&
	        begin <= range->chunks[range->rlen - 1].end)
	{
		range->vlen += end - range->chunks[range->rlen - 1].end;
		range->chunks[range->rlen - 1].end = end;
	}
	else
	{
		struct ent_range_chunk * chunks = realloc(
		                                      range->chunks,
		                                      sizeof(*chunks) * (range->rlen + 1));
		if (chunks == NULL)
		{
			return -1; // out of memory
		}

		chunks[range->rlen].begin = begin;
		chunks[range->rlen].end = end;
		range->chunks = chunks;
		range->rlen += 1;
		range->vlen += end - begin;
	}

	return 0;
}

/* This code is not being used right now but I put a lot of thought into it
 * and I expect it will come in handy as soon as I want to start combining
 * ranges.
int ent_range_delete (struct ent_range * range, struct ent_range const * src)
{
	struct ent_range * collect = ent_range_alloc();
	if (collect == NULL)
	{
		return -1;
	}

	int rc = 0;
	struct ent_range_chunk * old = range->chunks;
	struct ent_range_chunk * old_end =
			    range->chunks == NULL ? NULL : range->chunks + range->rlen;
	struct ent_range_chunk * del = src->chunks;
	struct ent_range_chunk * del_end =
			    src->chunks == NULL ? NULL : src->chunks + src->rlen;

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
				rc = ent_range_append(collect, old->begin, old->end);
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
				rc = ent_range_append(collect, old->begin, del->begin);
				if (rc == -1)
				{
					break;
				}
			}
			if (del->end < old->end)
			{
				rc = ent_range_append(collect, del->end, old->end);
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
			free(range->chunks);
			*range = *collect;
			free(collect);//Maybe collect can be on the stack instead?
		}
	}
	while (0);

	return rc;
}
*/
