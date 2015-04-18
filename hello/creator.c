#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ent.h"

typedef double mass;

struct creator
{
	struct ent_lock * lock;
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
		if (creator->lock)
		{
			ent_lock_free (creator->lock);
		}
		free (creator);
	}
}

int
creator_bind (struct creator * creator, struct ent_table * entities)
{
	if (!creator)
	{
		errno = EINVAL;
		return -1;
	}

	if (creator->lock)
	{
		ent_lock_free (creator->lock);
	}

	if (!entities)
	{
		return 0;
	}

	creator->entities = entities;

	creator->lock = ent_lock_alloc();

	if (!creator->lock)
	{
		return -1;
	}

	if (ent_lock_for_insert (
	            creator->lock, entities)
	        == -1)
	{
		ent_lock_free (creator->lock);
		creator->lock = NULL;
		return -1;
	}

	creator->mass =
	    ent_lock_for_update (
	        creator->lock, creator->entities, "mass", sizeof (mass));

	if (creator->mass == -1)
	{
		ent_lock_free (creator->lock);
		creator->lock = NULL;
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

	struct ent_session * session = ent_session_alloc (creator->lock);

	if (!session)
	{
		return -1;
	}

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	struct ent_table * new_entities =
	    ent_session_insert (session, creator->entities, appending);

	if (!new_entities)
	{
		ent_session_free (session);
		return -1;
	}

	// Get a non-const pointer to the appended "mass" data.
	mass * mass =
	    ent_session_update (session, new_entities, creator->mass);

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
