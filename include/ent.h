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

struct ent_bytes * ent_bytes_alloc ();
void ent_bytes_free (struct ent_bytes * b);
void const * ent_bytes_get (struct ent_bytes const * b, size_t * len);
int ent_bytes_reset (struct ent_bytes * b, void const * start, size_t n);

struct ent_range;

struct ent_range * ent_range_alloc ();
void ent_range_free (struct ent_range * range);
size_t ent_range_len (struct ent_range const * range);
int ent_range_append (struct ent_range * range, size_t begin, size_t end);
int ent_range_delete (struct ent_range * range, struct ent_range const * src);

struct ent_column;

struct ent_column * ent_column_alloc (struct ent_range * range, char const * name, char const * type);
void ent_column_free (struct ent_column * c);
void * ent_column_ref (struct ent_column * c);
const char * ent_column_type (struct ent_column * c);
struct ent_typeinfo const * ent_column_typeinfo (struct ent_column * c);

#endif//LIBENT_ENT_H
