#include "alloc.h"

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

static size_t fail_at = 0;
static size_t allocs = 0;

int
ent_alloc (void ** ptr, size_t size)
{
	if (!ptr)
	{
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

		errno = ENOMEM;
		return -1;
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

size_t ent_alloc_count()
{
	return allocs;
}

void ent_alloc_artificial_fail (size_t at_count)
{
	fail_at = at_count;
}
