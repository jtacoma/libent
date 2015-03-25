#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	void * start;
	struct ent_typeinfo type;
	size_t len;
};

struct ent_column *
ent_column_alloc (char const * type, size_t len)
{
	struct ent_column * column = calloc(1, sizeof(*column));
	if (column == NULL)
	{
		return NULL; // out of memory
	}

	if (ent_typeinfo_parse( &column->type, type) == -1)
	{
		free (column); // out of memory
		return NULL; // out of memory
	}

	if (len > 0)
	{
		column->start = calloc(len,
		                       ent_typeinfo_width(&column->type));
		if (column->start == NULL)
		{
			ent_column_free(column); // out of memory
			return NULL; // out of memory
		}

		column->len = len;
	}

	return column;
}

void ent_column_free(struct ent_column * c)
{
	if (c != NULL)
	{
		if (c->start != NULL)
		{
			if ( c->type.clear != NULL)
			{
				size_t width = ent_typeinfo_width(&c->type);
				uint8_t * ptr = c->start;
				uint8_t * end = ptr + c->len * width;
				while (ptr < end)
				{
					c->type.clear(ptr);
					ptr += width;
				}
			}
			free(c->start);
		}
		free(c);
	}
}

void const * ent_column_get (struct ent_column const * c, size_t * len)
{
	*len = c->len;
	return c->start;
}

struct ent_typeinfo const * ent_column_typeinfo (struct ent_column const * c)
{
	return &c->type;
}

void * ent_column_ref (struct ent_column * c, size_t *len)
{
	*len = c->len;
	return c->start;
}

int ent_column_select (struct ent_column * dst,
                       struct ent_column const * src,
                       struct ent_range const * range)
{
	struct ent_typeinfo const * dst_type = ent_column_typeinfo (dst);
	struct ent_typeinfo const * src_type = ent_column_typeinfo (src);
	if (!ent_typeinfo_equal(dst_type, src_type))
	{
		return -1; // mismatched types
	}
	size_t width = ent_typeinfo_width(dst_type);

	size_t dst_cap = 0;
	uint8_t * dst_ptr = ent_column_ref(dst, &dst_cap);
	uint8_t * dst_next = dst_ptr;

	size_t src_len = 0;
	uint8_t const * src_ptr = ent_column_get(src, &src_len);

	size_t chunks_len = 0;
	struct ent_range_chunk const * chunks =
	    ent_range_chunks(range, &chunks_len);

	size_t required_dst_len = ent_range_len (range);
	size_t required_src_len = chunks_len
	                          ? chunks[chunks_len - 1].end
	                          : 0;

	if (required_src_len > src_len)
	{
		return -1;
	}

	if (required_dst_len != dst_cap)
	{
		// Sometime this case might become a "replace" or "append"
		// operation, but for now it's just not acceptable.
		return -1;
	}

	for (size_t i = 0; i < chunks_len; ++i)
	{
		size_t n = (chunks[i].end - chunks[i].begin) * width;
		uint8_t const * src_chunk = src_ptr + chunks[i].begin * width;
		memcpy (dst_next, src_chunk, n);
		dst_next += n;
	}

	return 0;
}
