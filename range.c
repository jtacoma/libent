#include "ent.h"
#include "ent_internal.h"

#include <string.h>
#include <stdlib.h>

struct chunk
{
	size_t begin;
	size_t end;
};

struct ent_range
{
	size_t vlen;
	size_t rlen;
	struct chunk * chunks;
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

int ent_range_append (struct ent_range * range, size_t begin, size_t end)
{
	if (end <= begin || (range->chunks != NULL && begin < range->chunks[range->rlen - 1].begin))
	{
		return -1;
	}

	if (range->chunks != NULL && end <= range->chunks[range->rlen - 1].end)
	{
		return 0;
	}

	if (range->chunks != NULL && begin <= range->chunks[range->rlen - 1].end)
	{
		range->vlen += end - range->chunks[range->rlen - 1].end;
		range->chunks[range->rlen - 1].end = end;
	}
	else
	{
		struct chunk * chunks = realloc(range->chunks, sizeof(*chunks) * (range->rlen + 1));
		if (chunks == NULL)
		{
			return -1;
		}

		chunks[range->rlen].begin = begin;
		chunks[range->rlen].end = end;
		range->chunks = chunks;
		range->rlen += 1;
		range->vlen += end - begin;
	}

	return 0;
}

int ent_range_delete (struct ent_range * range, struct ent_range const * src)
{
	struct ent_range * collect = ent_range_alloc();
	if (collect == NULL)
	{
		return -1;
	}

	int rc = 0;
	struct chunk * old = range->chunks;
	struct chunk * old_end = range->chunks == NULL ? NULL : range->chunks + range->rlen;
	struct chunk * del = src->chunks;
	struct chunk * del_end = src->chunks == NULL ? NULL : src->chunks + src->rlen;

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
