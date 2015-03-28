#ifndef LIBENT_ENT_INTERNAL_H
#define LIBENT_ENT_INTERNAL_H

#include <stdbool.h>

// typeinfo

enum ent_datakind
{
	ENT_KIND_BUF = 1 << 4,
	ENT_KIND_FLOAT = 1 << 5,
	ENT_KIND_INT = 1 << 6,
	ENT_KIND_UINT = 1 << 7,
};

enum ent_elemtype
{
	ENT_TYPE_FLOAT8 = ENT_KIND_FLOAT + 1,
	ENT_TYPE_FLOAT16 = ENT_KIND_FLOAT + 2,
	ENT_TYPE_FLOAT32 = ENT_KIND_FLOAT + 4,
	ENT_TYPE_FLOAT64 = ENT_KIND_FLOAT + 8,
	ENT_TYPE_INT8 = ENT_KIND_INT + 1,
	ENT_TYPE_INT16 = ENT_KIND_INT + 2,
	ENT_TYPE_INT32 = ENT_KIND_INT + 4,
	ENT_TYPE_INT64 = ENT_KIND_INT + 8,
	ENT_TYPE_UINT8 = ENT_KIND_UINT + 1,
	ENT_TYPE_UINT16 = ENT_KIND_UINT + 2,
	ENT_TYPE_UINT32 = ENT_KIND_UINT + 4,
	ENT_TYPE_UINT64 = ENT_KIND_UINT + 8,
	ENT_TYPE_BYTES = ENT_KIND_BUF + 0,
	ENT_TYPE_UTF8 = ENT_KIND_BUF + 1,
};

struct ent_typeinfo
{
	char const *cname;
	enum ent_elemtype elem;
	size_t elemsize;
	size_t dimensions;
	size_t n;
};

int ent_typeinfo_parse (struct ent_typeinfo * typeinfo, char const * type);
bool ent_typeinfo_equal (struct ent_typeinfo const * typeinfo,
                         struct ent_typeinfo const * other);
char const * ent_typeinfo_name (struct ent_typeinfo const * typeinfo);
size_t ent_typeinfo_width (struct ent_typeinfo const * typeinfo);

// rlist

struct ent_rlist_range
{
	size_t begin;
	size_t end;
};

struct ent_rlist_range const * ent_rlist_ranges (
    struct ent_rlist const * rlist, size_t *len);

// column

struct ent_column * ent_column_alloc (char const * type, size_t len);
void ent_column_free (struct ent_column * c);
size_t ent_column_len (struct ent_column const * c);

// table

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

// model

bool ent_model_has (struct ent_model * m, char const * table_name);
struct ent_table * ent_model_get (struct ent_model * m, char const * table_name);

#endif//LIBENT_ENT_INTERNAL_H
