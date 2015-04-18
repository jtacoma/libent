#ifndef LIBENT_TABLE_H
#define LIBENT_TABLE_H

struct ent_table;

void
ent_table_incref (
    struct ent_table * table);

size_t
ent_table_columns_len (
    struct ent_table const * table);

size_t
ent_table_len (
    struct ent_table const * table);

struct ent_array *
ent_table_column (
    struct ent_table * table,
    struct ent_column const * column);

int
ent_table_delete (
    struct ent_table * table,
    struct ent_rlist const * rlist);

int
ent_table_grow (
    struct ent_table * table,
    size_t add);

int
ent_table_pre_grow (
    struct ent_table * table,
    size_t add);

int
ent_table_insert (
    struct ent_table * dst_table,
    struct ent_table * src_table);

#endif//LIBENT_TABLE_H
