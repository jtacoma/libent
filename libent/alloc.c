#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "alloc.h"

static size_t fail_at = 0;
static size_t allocs = 0;

int
ent_alloc (
    void ** ptr,
    size_t size)
{
	if (!ptr)
	{
		errno = EINVAL;
		return -1;
	}

	if (size)
	{
		allocs += 1;

		if (allocs == fail_at)
		{
			errno = ENOMEM;
			return -1;
		}

		if (*ptr)
		{
			void * newptr = realloc (*ptr, size);

			if (newptr)
			{
				*ptr = newptr;
				return 0;
			}
		}
		else if ((bool) (*ptr = malloc (size)))
		{
			return 0;
		}

		errno = ENOMEM; // out of memory (for real)
		return -1; // out of memory (for real)
	}
	else
	{
		if (*ptr)
		{
			free (*ptr);
			*ptr = NULL;
		}

		return 0;
	}
}

size_t
ent_alloc_count ()
{
	return allocs;
}

size_t ent_alloc_fail_at (void)
{
	return fail_at;
}

void
ent_alloc_set_fail_at (
    size_t at_count)
{
	fail_at = at_count;
}
