#ifndef ENTLIB_COLUMN_H
#define ENTLIB_COLUMN_H

struct ent_array;

struct ent_array *
ent_array_alloc (
    size_t width);

struct ent_array *
ent_array_cpy_alloc (
    struct ent_array const * src);

void
ent_array_free (
    struct ent_array * a);

void const *
ent_array_get (
    struct ent_array const * a);

size_t
ent_array_len (
    struct ent_array const * a);

size_t
ent_array_width (
    struct ent_array const * a);

void *
ent_array_ref (
    struct ent_array * a);

int
ent_array_set_len (
    struct ent_array * a,
    size_t add);

#endif//ENTLIB_COLUMN_H
