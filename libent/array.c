#include <errno.h>
#include <stdint.h> // for "array.h"
#include <string.h> // memcpy, memset

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "rlist.h"

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
		errno = EINVAL;
		return NULL;
	}

	struct ent_array * array = NULL;

	if (ent_alloc ((void**)&array, sizeof (*array)) == 0)
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
		errno = EINVAL;
		return NULL;
	}

	struct ent_array * dst = ent_array_alloc (src->width);

	if (!dst)
	{
		return NULL;
	}

	if (ent_array_set_len (dst, src->len) == -1)
	{
		ent_alloc ((void**)&dst, 0);
		return NULL;
	}

	memcpy (dst->start, src->start, src->width * src->len);

	return dst;
}

void
ent_array_free (
    struct ent_array * a)
{
	if (!a)
	{
		errno = EINVAL;
		return;
	}

	if (a->start)
	{
		ent_alloc (&a->start, 0);
	}

	ent_alloc ((void**)&a, 0);
}

void const *
ent_array_get_const (
    struct ent_array const * a)
{
	if (!a)
	{
		errno = EINVAL;
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
		errno = EINVAL;
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
		errno = EINVAL;
		return 0;
	}

	return a->width;
}

int
ent_array_set_len (
    struct ent_array * a,
    size_t len)
{
	if (!a)
	{
		errno = EINVAL;
		return -1;
	}

	if (ent_array_preallocate (a, len) == -1)
	{
		return -1;
	}

	if (len < a->len)
	{
		memset (
		    ((uint8_t*)a->start) + (a->width * len),
		    0,
		    a->width * (a->len - len));
	}

	a->len = len;
	return 0;
}

int
ent_array_preallocate (
    struct ent_array * a,
    size_t cap)
{
	if (!a)
	{
		errno = EINVAL;
		return -1;
	}

	if (cap > a->cap)
	{
		if (ent_alloc (&a->start, a->width * cap) == -1)
		{
			return -1;
		}

		size_t added = cap - a->cap;

		memset (
		    ((uint8_t*)a->start) + (a->width * a->cap),
		    0,
		    a->width * added);

		a->cap = cap;
	}

	return 0;
}

int
ent_array_shrink (
    struct ent_array * a)
{
	if (!a)
	{
		errno = EINVAL;
		return -1;
	}

	if (a->len == a->cap)
	{
		return 0;
	}

	// When a->len is zero, the effect will be that the memory is freed and
	// a->start becomes NULL.
	if (ent_alloc (&a->start, a->len * a->width) == -1)
	{
		return -1;
	}

	a->cap = a->len;

	return 0;
}

void *
ent_array_get (
    struct ent_array * a)
{
	if (!a)
	{
		errno = EINVAL;
		return NULL;
	}

	return a->start;
}

int
ent_array_select_in_place (
    struct ent_array * array,
    struct ent_rlist const * keep)
{
	size_t ranges_len;
	struct ent_rlist_range const * ranges = ent_rlist_ranges (keep, &ranges_len);

	if (ranges_len == 0)
	{
		ent_array_set_len (array, 0);
		return 0;
	}

	if (ranges[ranges_len - 1].end > array->len)
	{
		errno = EINVAL;
		return -1;
	}

	size_t next = 0;
	uint8_t * start = array->start;

	for (size_t i = 0; i < ranges_len; ++i)
	{
		if (ranges[i].begin == 0)
		{
			next = ranges[i].end;
			continue;
		}

		memmove (start + next * array->width,
		         start + ranges[i].begin * array->width,
		         (ranges[i].end - ranges[i].begin) * array->width);

		next += ranges[i].end - ranges[i].begin;
	}

	return 0;
}
