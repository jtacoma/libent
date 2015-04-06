#include "ent.h"
#include "alloc.h"
#include "table.h"
#include "processor.h"
#include "array.h"
#include "rlist.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define inserts_zero ((INT_MAX/4) * 3)

struct insertion
{
	struct ent_table * dst;
	struct ent_table * src;
};

ent_array_typed (struct insertion, insertion);

struct deletion
{
	struct ent_table * dst;
	struct ent_rlist * rlist;
};

ent_array_typed (struct deletion, deletion);

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
		ent_insertion_array_free (s->insertions);
		ent_alloc ((void**)&s, 0);
	}
}

size_t
ent_session_table_len (
    struct ent_session * s,
    struct ent_table * table)
{
	size_t len = 0;

	if (s && table)
	{
		len = ent_table_len (table);
	}

	return len;
}

struct ent_table *
ent_session_table_insert (
    struct ent_session * s,
    struct ent_table * table,
    size_t add)
{
	if (! (s && table && add))
	{
		return NULL;
	}

	struct ent_table * buffer = ent_table_alloc();

	if (!buffer)
	{
		return NULL;
	}

	if (ent_table_grow (buffer, add) == -1)
	{
		ent_table_free (buffer);
		return NULL;
	}

	size_t insertions_len = ent_insertion_array_len (s->insertions);

	if (-1 == ent_insertion_array_set_len (
	            s->insertions, insertions_len + 1))
	{
		ent_table_free (buffer);
		return NULL;
	}

	struct insertion * insertions = ent_insertion_array_get (s->insertions);

	insertions[insertions_len].dst = table;
	insertions[insertions_len].src = buffer;

	ent_table_incref (buffer);
	return buffer;
}

int
ent_session_table_delete (
    struct ent_session * s,
    struct ent_table * table,
    struct ent_rlist const * rlist)
{
	if (! (s && rlist && table))
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
    struct ent_table * table,
    int column_id)
{
	void * mem = NULL;

	if (s)
	{
		struct column_info const column_info =
		    ent_processor_column (s->processor, column_id);

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
    struct ent_table * table,
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
		assert (ent_table_delete (deletions[i].dst, deletions[i].rlist) == 0);
	}

	size_t inserts_len = ent_insertion_array_len (s->insertions);
	struct insertion const * insertions = ent_insertion_array_get_const (s->insertions);

	for (size_t i = 0; i < inserts_len; ++i)
	{
		struct ent_table * dst_table = insertions[i].dst;
		struct ent_table * src_table = insertions[i].src;

		assert (ent_table_insert (dst_table, src_table) == 0);
	}

	return 0;
}
