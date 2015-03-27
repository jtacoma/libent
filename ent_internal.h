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

/* Logically equivalent to realloc from stdlib.h plus initialization.
 *
 * If zero is true and the new size is older than the old size, the additional
 * memory is set to zero.
 */
void * ent_realloc (void * ptr, size_t size, bool zero);

#define ent_realloc_array(ptr, nmemb, zero) \
	ent_realloc ((ptr), (nmemb) * sizeof(*(ptr)), (zero))

#define ent_realloc_carray(ptr, nmemb, size, zero) \
	ent_realloc ((ptr), (nmemb) * (size), (zero))

#define ent_realloc_free(ptr) \
	ent_realloc ((ptr), 0, false)

#endif//LIBENT_ENT_INTERNAL_H
