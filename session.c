#include "ent.h"
#include "model.h"
#include "table.h"
#include "processor.h"
#include "array.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define inserts_zero ((INT_MAX/4) * 3)

struct insert
{
	struct ent_table * dst;
	struct ent_table * src;
};

struct ent_session
{
	struct ent_processor const * processor;
	bool locked;
	struct ent_array * inserts;
};

struct ent_session *
ent_session_alloc (
    struct ent_processor * p)
{
	if (!p)
	{
		return NULL;
	}

	struct ent_session * s = malloc (sizeof (*s));

	if (s)
	{
		*s = (struct ent_session) {.processor = p};
		s->inserts = ent_array_alloc (sizeof (struct insert));
		if (!s->inserts)
		{
			free (s);
			s = NULL;
		}
	}

	return s;
}

void
ent_session_free (
    struct ent_session * s)
{
	if (s)
	{
		free (s->inserts);
		free (s);
	}
}

int
ent_session_lock (
    struct ent_session * s)
{
	if (!s)
	{
		return -1;
	}

	s->locked = true;
	return 0;
}

size_t
ent_session_table_len (
    struct ent_session * s,
    int table_id)
{
	size_t len = 0;

	if (s)
	{
		struct ent_table * table = ent_processor_table (
		                               s->processor,
		                               table_id);

		if (table)
		{
			len = ent_table_len (table);
		}
	}

	return len;
}

int
ent_session_table_insert (
    struct ent_session * s,
    int table_id,
    size_t add)
{
	if (!s || !s->locked || !add)
	{
		return -1;
	}

	struct ent_table * existing = ent_processor_table (s->processor, table_id);

	if (!existing)
	{
		return -1;
	}

	struct ent_table * buffer = ent_table_alloc (add);

	if (buffer == NULL)
	{
		return -1;
	}

	size_t inserts_len = ent_array_len (s->inserts);
	if (ent_array_set_len (s->inserts, inserts_len + 1) == -1)
	{
		ent_table_decref (buffer);
		return -1;
	}

	struct insert * inserts = ent_array_ref (s->inserts);
	inserts[inserts_len].dst = existing;
	inserts[inserts_len].src = buffer;
	return inserts_zero + (int) inserts_len;
}

void *
ent_session_column_write (
    struct ent_session * s,
    int table_id,
    int column_id)
{
	void * mem = NULL;

	if (s)
	{
		struct column_info const column_info = ent_processor_column (
		        s->processor,
		        column_id);

		struct ent_table * table = ent_processor_table (
		                               s->processor,
		                               table_id);

		struct ent_array * array = ent_table_column (
		                               table,
		                               column_info.name,
		                               column_info.width);

		if (array)
		{
			mem = ent_array_ref (array);
		}
	}

	return mem;
}

void const *
ent_session_column_read (
    struct ent_session * s,
    int table,
    int column)
{
	return ent_session_column_write (s, table, column);
}

int
ent_session_commit (
    struct ent_session * s)
{
	if (!s)
	{
		return -1;
	}

	size_t inserts_len = ent_array_len (s->inserts);
	struct insert const * inserts = ent_array_get (s->inserts);

	for (size_t i = 0; i < inserts_len; ++i)
	{
		struct ent_table * dst_table = inserts[i].dst;
		struct ent_table * src_table = inserts[i].src;

		size_t start = ent_table_len (dst_table);

		ent_table_grow (dst_table, ent_table_len (src_table));

		size_t columns_len = ent_table_columns_len (src_table);

		for (size_t k = 0; k < columns_len; ++k)
		{
			size_t width;
			char const * name =
			    ent_table_column_info (src_table, k, &width);

			struct ent_array * src_array =
			    ent_table_column (src_table, name, width);

			struct ent_array * dst_array =
			    ent_table_column (dst_table, name, width);

			if (dst_array == NULL)
			{
				// um....
			}

			uint8_t * dst = ent_array_ref (dst_array);

			dst += width * start;

			uint8_t const * src = ent_array_get (src_array);

			memcpy (dst, src, width * ent_table_len (dst_table));
		}
	}

	return 0;
}

