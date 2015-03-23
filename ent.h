#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ent_typeinfo;
struct ent_typeinfo const * ent_typeinfo_from_name (char const * name);
size_t ent_typeinfo_width (struct ent_typeinfo const * typeinfo);

struct ent_column;
struct ent_column * ent_column_alloc (char const * name, struct ent_typeinfo const * type);
void ent_column_start (struct ent_column * c, void ** start, size_t * n);
struct ent_typeinfo const * ent_column_type (struct ent_column * c);
void ent_column_free (struct ent_column * c);

struct ent_insert_buffer * ent_insert_buffer_alloc (struct ent_column * column);
int ent_insert_buffer_append (struct ent_insert_buffer * b, void * start, size_t n);
int ent_insert_buffer_commit (struct ent_insert_buffer * b);
void ent_insert_buffer_free (struct ent_insert_buffer * b);

struct ent_update_buffer * ent_update_buffer_alloc (struct ent_column * column);
int ent_update_buffer_set (struct ent_update_buffer * b, size_t dst, void * src, size_t n);
int ent_update_buffer_commit (struct ent_update_buffer * b);
void ent_update_buffer_free (struct ent_update_buffer * b);

struct ent_delete_buffer * ent_delete_buffer_alloc (struct ent_column * column);
int ent_delete_buffer_rm (struct ent_delete_buffer * b, size_t start, size_t n);
int ent_delete_buffer_commit (struct ent_delete_buffer * b);
void ent_delete_buffer_free (struct ent_delete_buffer * b);

#endif//LIBENT_ENT_H
