#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ent_rlist;

struct ent_rlist * ent_rlist_alloc();
void ent_rlist_free (struct ent_rlist * rlist);
size_t ent_rlist_len (struct ent_rlist const * rlist);
int ent_rlist_append (struct ent_rlist * rlist,
                      size_t begin,
                      size_t end);
int ent_rlist_delete (struct ent_rlist * rlist,
                      struct ent_rlist const * src);

struct ent_model;
struct ent_session;
struct ent_table;
struct ent_column;

struct ent_model * ent_model_alloc();
void ent_model_free (struct ent_model * m);

struct ent_session * ent_session_alloc (struct ent_model * m);
void ent_session_free (struct ent_session * s);
int ent_session_lock (struct ent_session * s);
struct ent_table * ent_session_table (struct ent_session * s,
                                      char const * table_name);
struct ent_column const *
ent_session_column_r (struct ent_session * s,
                      struct ent_table * table,
                      char const * column_name,
                      char const * type);
struct ent_column *
ent_session_column_w (struct ent_session * s,
                      struct ent_table * table,
                      char const * column_name,
                      char const * type);
size_t ent_session_table_len (struct ent_session * s,
                              struct ent_table * table);
int ent_session_table_grow (struct ent_session * s,
                            struct ent_table * table,
                            size_t add_size);

void const * ent_column_get (struct ent_column const * c);
struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c);
char const * ent_column_typename (struct ent_column const * c);
void * ent_column_ref (struct ent_column * c);
int ent_column_select (struct ent_column * c,
                       struct ent_column const * src,
                       struct ent_rlist const * rlist);

#endif//LIBENT_ENT_H
