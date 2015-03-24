#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_bytes * ent_bytes_alloc ()
{
	return calloc(1, sizeof(struct ent_bytes));
}

void ent_bytes_free (struct ent_bytes * b)
{
	if (b != NULL)
	{
		if (b->p != NULL)
		{
			free(b->p);
		}

		free (b);
	}
}

void const * ent_bytes_get (struct ent_bytes const * b, size_t * len)
{
	*len = b->len;
	return b->p;
}

int ent_bytes_reset (struct ent_bytes * b, void const * src, size_t len)
{
	void * new_start = malloc(len);

	if (new_start == NULL)
	{
		return -1;
	}

	memcpy (new_start, src, len);
	b->p = new_start;
	b->len = len;
	return 0;
}

