#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

void const * ent_bytes_get (struct ent_bytes const * b, size_t * len)
{
	*len = b->len;
	return b->p;
}

void ent_bytes_clear (void * b)
{
	ent_bytes_reset((struct ent_bytes *) b, NULL, 0);
}

int ent_bytes_reset (struct ent_bytes * b, void const * src, size_t len)
{
	if (len == 0)
	{
		if (b->p != NULL)
		{
			free(b->p);
			b->p = NULL;
		}

		b->len = 0;
	}
	else
	{
		void * new_start = malloc(len);

		if (new_start == NULL)
		{
			return -1; // out of memory
		}

		memcpy (new_start, src, len);
		if (b->p != NULL)
		{
			free(b->p);
		}
		b->p = new_start;
		b->len = len;
	}
	return 0;
}

