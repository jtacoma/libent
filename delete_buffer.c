#include "ent.h"

struct ent_delete_buffer * ent_delete_buffer_alloc (struct ent_column * column)
{
	(void)column;
	return NULL;
}

int ent_delete_buffer_rm (struct ent_delete_buffer * b, size_t start, size_t n)
{
	(void)b;
	(void)start;
	(void)n;
	return -1;
}

int ent_delete_buffer_commit (struct ent_delete_buffer * b)
{
	(void)b;
	return -1;
}

void ent_delete_buffer_free (struct ent_delete_buffer * b)
{
	(void)b;
	// TODO
}
