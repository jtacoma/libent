#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ent_bytes
{
	void * p;
	size_t len;
};

void const * ent_bytes_get (struct ent_bytes const * b, size_t * len);
int ent_bytes_reset (struct ent_bytes * b, void const * start, size_t n);

struct ent_range;

struct ent_range * ent_range_alloc ();
void ent_range_free (struct ent_range * range);
size_t ent_range_len (struct ent_range const * range);
int ent_range_append (struct ent_range * range, size_t begin, size_t end);
int ent_range_delete (struct ent_range * range, struct ent_range const * src);

struct ent_column;

struct ent_column * ent_column_alloc (char const * type, size_t len);
void ent_column_free (struct ent_column * c);
void const * ent_column_get (struct ent_column const * c, size_t *len);
struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c);
void * ent_column_ref (struct ent_column * c, size_t *len);
int ent_column_select (struct ent_column * c,
		struct ent_column const * src,
		struct ent_range const * range);

#endif//LIBENT_ENT_H
