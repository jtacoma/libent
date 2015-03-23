#include "ent.h"
struct ent_update_buffer * ent_update_buffer_alloc (struct ent_column * column)
{
	(void)column;
	return NULL;
}
int ent_update_buffer_set (struct ent_update_buffer * b, size_t dst, void * src, size_t n)
{
	(void)b;
	(void)dst;
	(void)src;
	(void)n;
	return -1;
}
int ent_update_buffer_commit (struct ent_update_buffer * b)
{
	(void)b;
	return -1;
}
void ent_update_buffer_free (struct ent_update_buffer * b)
{
	(void)b;
	// TODO
}
