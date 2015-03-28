#ifndef ENTLIB_COLUMN_H
#define ENTLIB_COLUMN_H

struct ent_column * ent_column_alloc (char const * type, size_t len);
void ent_column_free (struct ent_column * c);
size_t ent_column_len (struct ent_column const * c);

#endif//ENTLIB_COLUMN_H
