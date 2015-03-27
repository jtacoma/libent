#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ent_rlist;

struct ent_rlist * ent_rlist_alloc();
void ent_rlist_free (struct ent_rlist * rlist);
size_t ent_rlist_len (struct ent_rlist const * rlist);
int ent_rlist_append (struct ent_rlist * rlist, size_t begin, size_t end);
int ent_rlist_delete (struct ent_rlist * rlist, struct ent_rlist const * src);

struct ent_column;

void const * ent_column_get (struct ent_column const * c, size_t *len);
struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c);
char const * ent_column_typename (struct ent_column const * c);
void * ent_column_ref (struct ent_column * c, size_t *len);
int ent_column_select (struct ent_column * c,
                       struct ent_column const * src,
                       struct ent_rlist const * rlist);

struct ent_table;

struct ent_table * ent_table_alloc (size_t len);
void ent_table_free (struct ent_table * table);
size_t ent_table_len (struct ent_table const * table);
struct ent_column * ent_table_add_column (struct ent_table * table,
        char const * name, char const * type);
struct ent_column * ent_table_column (struct ent_table * table,
                                      char const * name, char const * type);
int ent_table_delete (struct ent_table * table,
                      struct ent_rlist const * rlist);

#endif//LIBENT_ENT_H
