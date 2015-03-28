#ifndef LIBENT_TABLE_H
#define LIBENT_TABLE_H

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
int ent_table_grow (struct ent_table * table,
                    size_t add);

#endif//LIBENT_TABLE_H
