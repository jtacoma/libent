#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

void * ent_realloc (void * ptr, size_t size, bool zero)
{
	// Regions of memory are prefixed with their size to implement
	// additional realloc()-like semantics, specifically filling additional
	// bytes with zero.

	// Get the originally allocated pointer
	size_t * prior = ptr;
	if (prior != NULL)
	{
		prior--;
	}

	// Like realloc, size==0 means free()
	if (size == 0)
	{
		if (prior != NULL)
		{
			free (prior);
		}

		return NULL;
	}

	// Get the size of the originally allocated block
	size_t old_size = prior == NULL ? 0 : *prior;

	size_t * result = realloc (prior, size + sizeof (size_t));

	if (result != NULL)
	{
		result[0] = size;
		result ++;

		if (zero && old_size < size)
		{
			memset (result + (old_size / sizeof (*result)),
			        0, size - old_size);
		}
	}

	return result;
}
