#ifndef LIBENT_TABLE_H
#define LIBENT_TABLE_H

struct ent_table;

struct ent_table *
ent_table_alloc (
    size_t len);

void
ent_table_incref (
    struct ent_table * table);

void
ent_table_free (
    struct ent_table * table);

size_t
ent_table_columns_len (
    struct ent_table const * table);

char const *
ent_table_column_info (
    struct ent_table const * table,
    size_t column_index,
    size_t * width);

size_t
ent_table_len (
    struct ent_table const * table);

struct ent_array *
ent_table_column (
    struct ent_table * table,
    char const * name,
    size_t width);

int
ent_table_delete (
    struct ent_table * table,
    struct ent_rlist const * rlist);

int
ent_table_grow (
    struct ent_table * table,
    size_t add);

#endif//LIBENT_TABLE_H
