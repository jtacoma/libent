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

struct ent_rlist;

struct ent_rlist * ent_rlist_alloc ();
void ent_rlist_free (struct ent_rlist * rlist);
size_t ent_rlist_len (struct ent_rlist const * rlist);
int ent_rlist_append (struct ent_rlist * rlist, size_t begin, size_t end);
int ent_rlist_delete (struct ent_rlist * rlist, struct ent_rlist const * src);

struct ent_column;

struct ent_column * ent_column_alloc (char const * type, size_t len);
void ent_column_free (struct ent_column * c);
void const * ent_column_get (struct ent_column const * c, size_t *len);
struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c);
void * ent_column_ref (struct ent_column * c, size_t *len);
int ent_column_select (struct ent_column * c,
		struct ent_column const * src,
		struct ent_rlist const * rlist);

#endif//LIBENT_ENT_H
