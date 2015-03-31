#ifndef ENTLIB_COLUMN_H
#define ENTLIB_COLUMN_H

struct ent_column *
ent_column_alloc (
    size_t width);

void
ent_column_free (
    struct ent_column * c);

size_t
ent_column_len (
    struct ent_column const * c);

int
ent_column_grow (
    struct ent_column * c,
    size_t add);

#endif//ENTLIB_COLUMN_H
