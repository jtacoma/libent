#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_update_buffer
{
	struct ent_column * column;
	void * mem;
	bool done;
};

struct ent_update_buffer * ent_update_buffer_alloc (struct ent_column * column)
{
	struct ent_update_buffer * b = calloc(1, sizeof(*b));
	b->column = column;

	void * start;
	size_t n;
	ent_column_start(column, &start, &n);

	struct ent_typeinfo const * type = ent_column_type(column);
	size_t width = ent_typeinfo_width(type);
	b->mem = malloc(width * n);

	memcpy(b->mem, start, width * n);

	return b;
}

int ent_update_buffer_set (struct ent_update_buffer * b, size_t dst, void * src, size_t n)
{
	if (b->done)
	{
		return -1;
	}

	struct ent_typeinfo const * type = ent_column_type(b->column);
	size_t width = ent_typeinfo_width(type);
	memcpy(((int8_t *)b->mem) + dst * width, src, n * width);

	return 0;
}

int ent_update_buffer_commit (struct ent_update_buffer * b)
{
	if (b->done)
	{
		return -1;
	}

	void * start;
	size_t n;
	ent_column_start(b->column, &start, &n);

	ent_column_set(b->column, b->mem, n);

	b->done = true;

	return 0;
}

void ent_update_buffer_free (struct ent_update_buffer * b)
{
	(void)b;
	// TODO
}
