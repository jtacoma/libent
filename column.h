#ifndef ENTLIB_COLUMN_H
#define ENTLIB_COLUMN_H

struct ent_column;

struct ent_column *
ent_column_alloc (
    size_t width);

void
ent_column_free (
    struct ent_column * c);

void const *
ent_column_get (
    struct ent_column const * c);

size_t
ent_column_len (
    struct ent_column const * c);

size_t
ent_column_width (
    struct ent_column const * c);

void *
ent_column_ref (
    struct ent_column * c);

int
ent_column_set_len (
    struct ent_column * c,
    size_t add);

#endif//ENTLIB_COLUMN_H
