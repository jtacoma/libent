#include "ent.h"
#include "alloc.h"
#include "model.h"
#include "table.h"
#include "processor.h"
#include "array.h"
#include "rlist.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define inserts_zero ((INT_MAX/4) * 3)

typedef struct insertion
{
	struct ent_table * dst;
	struct ent_table * src;
} insertion;

ent_array_typed (insertion);

typedef struct deletion
{
	struct ent_table * dst;
	struct ent_rlist * rlist;
} deletion;

ent_array_typed (deletion);

struct ent_session
{
	struct ent_processor const * processor;
	struct ent_insertion_array * insertions;
	struct ent_deletion_array * deletions;
};

struct ent_session *
ent_session_alloc (
    struct ent_processor * p)
{
	if (!p)
	{
		return NULL;
	}

	struct ent_session * s = NULL;

	if (ent_alloc ((void**)&s, sizeof (*s)) == -1)
	{
		return NULL;
	}

	*s = (struct ent_session) {.processor = p};
	s->insertions = ent_insertion_array_alloc (sizeof (struct insertion));

	if (!s->insertions)
	{
		ent_alloc ((void**)&s, 0);
		return NULL;
	}

	s->deletions = ent_deletion_array_alloc();

	if (!s->deletions)
	{
		ent_insertion_array_free (s->insertions);
		ent_alloc ((void**)&s, 0);
		return NULL;
	}

	return s;
}

void
ent_session_free (
    struct ent_session * s)
{
	if (s)
	{
		size_t deletions_len = ent_deletion_array_len (s->deletions);
		struct deletion * deletions =
		    ent_deletion_array_get (s->deletions);

		for (size_t i = 0; i < deletions_len; ++i)
		{
			ent_rlist_free (deletions[i].rlist);
		}

		ent_deletion_array_free (s->deletions);

		size_t insertions_len = ent_insertion_array_len (s->insertions);
		struct insertion * insertions = ent_insertion_array_get (s->insertions);

		for (size_t i = 0; i < insertions_len; ++i)
		{
			ent_table_free (insertions[i].src);
		}

		ent_insertion_array_free (s->insertions);
		ent_alloc ((void**)&s, 0);
	}
}

static struct ent_table *
ent_session_table (
    struct ent_session const * s,
    int table_id)
{
	struct ent_table * table = NULL;

	if (s)
	{
		size_t index = (size_t) table_id;
		size_t tables_len = ent_processor_tables_len (s->processor);
		if (index < tables_len)
		{
			table = ent_processor_table (
			            s->processor,
			            table_id);
		}
		else
		{
			index -= tables_len;
			size_t insertions_len =
			    ent_insertion_array_len (s->insertions);
			if (index < insertions_len)
			{
				struct insertion const * insertions =
				    ent_insertion_array_get_const (s->insertions);
				table = insertions[index].src;
			}
		}
	}

	return table;
}

size_t
ent_session_table_len (
    struct ent_session * s,
    int table_id)
{
	size_t len = 0;

	if (s)
	{
		struct ent_table * table =
		    ent_session_table (s, table_id);

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
	if (!s || !add)
	{
		return -1;
	}

	struct ent_table * existing =
	    ent_processor_table (s->processor, table_id);

	if (!existing)
	{
		return -1;
	}

	struct ent_table * buffer = ent_table_alloc();

	if (!buffer)
	{
		return -1;
	}

	if (ent_table_grow (buffer, add) == -1)
	{
		ent_table_free (buffer);
		return -1;
	}

	size_t insertions_len = ent_insertion_array_len (s->insertions);

	if (-1 == ent_insertion_array_set_len (
	            s->insertions, insertions_len + 1))
	{
		ent_table_free (buffer);
		return -1;
	}

	struct insertion * insertions = ent_insertion_array_get (s->insertions);

	insertions[insertions_len].dst = existing;
	insertions[insertions_len].src = buffer;

	size_t processor_tables_len = ent_processor_tables_len (s->processor);

	return (int) (processor_tables_len + insertions_len);
}

int
ent_session_table_delete (
    struct ent_session * s,
    int table_id,
    struct ent_rlist const * rlist)
{
	if (!s || !rlist)
	{
		return -1;
	}

	struct ent_table * table =
	    ent_processor_table (s->processor, table_id);

	if (!table)
	{
		return -1;
	}

	// TODO: copy rlist into rlist_cpy
	struct ent_rlist * rlist_cpy = ent_rlist_alloc();
	size_t ranges_len;
	struct ent_rlist_range const * ranges =
	    ent_rlist_ranges (rlist, &ranges_len);

	for (size_t i = 0; i < ranges_len; ++i)
	{
		ent_rlist_append (rlist_cpy, ranges[i].begin, ranges[i].end);
	}

	size_t deletions_len = ent_deletion_array_len (s->deletions);

	if (ent_deletion_array_set_len (s->deletions, deletions_len + 1) == -1)
	{
		return -1;
	}

	struct deletion * deletions =
	    ent_deletion_array_get (s->deletions);

	deletions[deletions_len].dst = table;
	deletions[deletions_len].rlist = rlist_cpy;
	return 0;
}

void *
ent_session_column_get (
    struct ent_session * s,
    int table_id,
    int column_id)
{
	void * mem = NULL;

	if (s)
	{
		struct column_info const column_info =
		    ent_processor_column (s->processor, column_id);

		struct ent_table * table =
		    ent_session_table (s, table_id);

		struct ent_array * array =
		    ent_table_column (
		        table,
		        column_info.name,
		        column_info.width);

		if (array)
		{
			mem = ent_array_get (array);
		}
	}

	return mem;
}

void const *
ent_session_column_get_const (
    struct ent_session * s,
    int table,
    int column)
{
	return ent_session_column_get (s, table, column);
}

int
ent_session_commit (
    struct ent_session * s)
{
	if (!s)
	{
		return -1;
	}

	size_t deletions_len = ent_deletion_array_len (s->deletions);
	struct deletion * deletions = ent_deletion_array_get (s->deletions);

	for (size_t i = 0; i < deletions_len; ++i)
	{
		if (-1 == ent_table_delete (
		            deletions[i].dst, deletions[i].rlist))
		{
			// atomicity violation!
			return -1;
		}
	}

	size_t inserts_len = ent_insertion_array_len (s->insertions);
	struct insertion const * insertions = ent_insertion_array_get_const (s->insertions);

	for (size_t i = 0; i < inserts_len; ++i)
	{
		struct ent_table * dst_table = insertions[i].dst;
		struct ent_table * src_table = insertions[i].src;

		size_t start = ent_table_len (dst_table);

		if (-1 == ent_table_grow (
		            dst_table, ent_table_len (src_table)))
		{
			// atomicity violation!
			return -1;
		}

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
				// atomicity violation!
				return -1;
			}

			uint8_t * dst = ent_array_get (dst_array);

			dst += width * start;

			uint8_t const * src = ent_array_get_const (src_array);

			memcpy (dst, src, width * ent_table_len (src_table));
		}
	}

	return 0;
}
