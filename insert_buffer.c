#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_insert_buffer
{
	struct ent_column * column;
	void * start;
	size_t n;
	bool done;
};

struct ent_insert_buffer * ent_insert_buffer_alloc (struct ent_column * column)
{
	struct ent_insert_buffer * b = calloc(1, sizeof(*b));
	b->column = column;
	return b;
}

int ent_insert_buffer_append (struct ent_insert_buffer * b, void * src, size_t n)
{
	if (b->done)
	{
		return -1;
	}

	struct ent_typeinfo const * type = ent_column_type(b->column);
	size_t width = ent_typeinfo_width(type);
	void * new_start = realloc(b->start, width * (b->n + n));

	if (new_start == NULL)
	{
		return -1;
	}

	memcpy(((int8_t *)new_start) + width * b->n, src, width * n);
	b->start = new_start;
	b->n += n;
	return 0;
}

int ent_insert_buffer_commit (struct ent_insert_buffer * b)
{
	if (b->done)
	{
		return -1;
	}

	b->done = true;
	size_t width = ent_typeinfo_width(ent_column_type(b->column));
	void * old_start;
	size_t old_n;
	ent_column_start(b->column, &old_start, &old_n);
	void * newstart = realloc(old_start, width * (old_n + b->n));

	if (newstart == NULL)
	{
		return -1; // out of memory
	}

	memcpy(((int8_t *)newstart) + width * old_n, b->start, width * b->n);
	return ent_column_set(b->column, newstart, old_n + b->n);
}

void ent_insert_buffer_free (struct ent_insert_buffer * b)
{
	if (b != NULL)
	{
		if (b->start != NULL)
		{
			free(b->start);
		}

		free(b);
	}
}

