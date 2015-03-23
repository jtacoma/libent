#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_insert_buffer
{
	struct ent_column * column;
	void * start;
	size_t n;
};

struct ent_insert_buffer * ent_insert_buffer_alloc (struct ent_column * column)
{
	struct ent_insert_buffer * b = calloc(1, sizeof(*b));
	b->column = column;
	return b;
}

int ent_insert_buffer_append (struct ent_insert_buffer * b, void * src, size_t n)
{
	size_t width = ent_typeinfo_width(ent_column_type(b->column));
	void * new_start = realloc(b->start, width * (b->n + n));
	if (new_start == NULL)
	{
		return -1;
	}
	memcpy(((int8_t *)new_start) + b->n, src, n);
	b->start = new_start;
	return 0;
}

int ent_insert_buffer_commit (struct ent_insert_buffer * b)
{
	// TODO: add items to b->column
	(void)b;
	return -1;
}

void ent_insert_buffer_free (struct ent_insert_buffer * b)
{
	free(b->start);
	free(b);
}

