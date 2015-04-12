#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ent.h"

struct creator
{
	struct ent_processor * processor;
	struct ent_table * entities;
	int mass;
};

struct creator *
creator_alloc()
{
	return calloc (1, sizeof (struct creator));
}

void
creator_free (struct creator * creator)
{
	if (creator)
	{
		if (creator->processor)
		{
			ent_processor_free (creator->processor);
		}
		free (creator);
	}
}

#define ent_typedef_column(T, N) \
	static inline int ent_processor_use_ ## N ## _column ( \
		struct ent_processor * processor, \
		struct ent_table * table, \
		char const * mode) \
	{ \
		return ent_processor_use_column ( \
			processor, table, #N, sizeof (T), mode); \
	} \
	typedef T N

ent_typedef_column (double, mass);

int
creator_bind (struct creator * creator, struct ent_table * entities)
{
	if (!creator)
	{
		errno = EINVAL;
		return -1;
	}

	if (creator->processor)
	{
		ent_processor_free (creator->processor);
	}

	if (!entities)
	{
		return 0;
	}

	creator->entities = entities;

	creator->processor = ent_processor_alloc();

	if (!creator->processor)
	{
		return -1;
	}

	if (ent_processor_use_table (
	            creator->processor, entities, "w")
	        == -1)
	{
		ent_processor_free (creator->processor);
		creator->processor = NULL;
		return -1;
	}

	creator->mass =
	    ent_processor_use_mass_column (
	        creator->processor, creator->entities,  "w");

	if (creator->mass == -1)
	{
		ent_processor_free (creator->processor);
		creator->processor = NULL;
		return -1;
	}

	return 0;
}


int
creator_execute (struct creator * creator, size_t appending)
{
	if (!creator)
	{
		errno = EINVAL;
		return -1;
	}

	struct ent_session * session = ent_session_alloc (creator->processor);

	if (!session)
	{
		return -1;
	}

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	struct ent_table * new_entities =
	    ent_session_table_insert (session, creator->entities, appending);

	if (!new_entities)
	{
		ent_session_free (session);
		return -1;
	}

	// Get a non-const pointer to the appended "mass" data.
	mass * mass =
	    ent_session_column_get (session, new_entities, creator->mass);

	if (!mass)
	{
		ent_session_free (session);
		return -1;
	}

	// This array of float64 values can be used as normal.
	for (size_t i = 0; i < appending; ++i)
	{
		mass[i] = (double) (i * i) / 3;
	}

	assert (ent_session_commit (session) == 0);

	ent_session_free (session);

	return 0;
}