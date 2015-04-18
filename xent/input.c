#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <ent.h>

#include "xent.h"

struct input
{
	struct ent_table * entities;
	struct ent_lock * lock;
	int type;
	int id;
	int pos;
	int dead;
};

struct input *
input_alloc (
    struct ent_table * entities)
{
	if (!entities)
	{
		errno = EINVAL;
		return NULL;
	}

	struct input * input = calloc (1, sizeof (*input));
	if (!input)
	{
		return NULL;
	}

	input->lock = ent_lock_alloc();
	if (!input->lock)
	{
		free (input);
		return NULL;
	}

	input->type =
	    ent_lock_for_update (
	        input->lock, entities, "input_type", sizeof (enum input_type));
	if (input->type == -1)
	{
		ent_lock_free (input->lock);
		free (input);
		return NULL;
	}

	input->id =
	    ent_lock_for_update (
	        input->lock, entities, "input_id", sizeof (input_id));
	if (input->id == -1)
	{
		ent_lock_free (input->lock);
		free (input);
		return NULL;
	}

	input->pos =
	    ent_lock_for_update (
	        input->lock, entities, "pos", sizeof (length_xy));
	if (input->pos == -1)
	{
		ent_lock_free (input->lock);
		free (input);
		return NULL;
	}

	input->dead =
	    ent_lock_for_update (
	        input->lock, entities, "dead", sizeof (uint8_t));
	if (input->dead == -1)
	{
		ent_lock_free (input->lock);
		free (input);
		return NULL;
	}

	input->entities = entities;

	return input;
}

void
input_free (
    struct input * input)
{
	free (input);
}

int
input_begin (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos)
{
	int rc = -1;

	struct ent_session * session =
	    ent_session_alloc (input->lock);
	if (!session)
	{
		return rc;
	}

	do
	{
		printf ("input!\n");

		struct ent_table * one =
		    ent_session_insert (session, input->entities, 1);
		if (!one)
		{
			break;
		}

		enum input_type * types =
		    ent_session_update (session, one, input->type);
		if (!types)
		{
			break;
		}
		types[0] = type;

		input_id * ids =
		    ent_session_update (session, one, input->id);
		if (!ids)
		{
			break;
		}
		ids[0] = id;

		length_xy * poss =
		    ent_session_update (session, one, input->pos);
		if (!poss)
		{
			break;
		}

		memcpy (
		    poss[0],
		    pos,
		    sizeof (length_xy));

		rc = ent_session_commit (session);
	}
	while (0);

	ent_session_free (session);
	return rc;
}

int
input_update (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos)
{
	int rc = -1;

	struct ent_session * session =
	    ent_session_alloc (input->lock);
	if (!session)
	{
		return rc;
	}

	struct ent_table * entities = input->entities;

	do
	{
		printf ("input!\n");

		enum input_type const * types =
		    ent_session_update (session, entities, input->type);
		if (!types)
		{
			break;
		}

		input_id const * ids =
		    ent_session_update (session, entities, input->id);
		if (!ids)
		{
			break;
		}

		length_xy * poss =
		    ent_session_update (session, entities, input->pos);
		if (!poss)
		{
			break;
		}

		size_t len = ent_session_table_len (session, entities);
		for (size_t i = 0; i < len; ++i)
		{
			if (types[i] == type && ids[i] == id)
			{
				memcpy (
				    poss[i],
				    pos,
				    sizeof (length_xy));
			}
		}

		rc = ent_session_commit (session);
	}
	while (0);

	ent_session_free (session);

	if (rc == -1)
	{
		rc = input_begin (input, type, id, pos);
	}

	return rc;
}

int
input_end (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos)
{
	int rc = -1;

	struct ent_session * session =
	    ent_session_alloc (input->lock);
	if (!session)
	{
		return rc;
	}

	struct ent_table * entities = input->entities;

	do
	{
		printf ("input!\n");

		enum input_type const * types =
		    ent_session_update (session, entities, input->type);
		if (!types)
		{
			break;
		}

		input_id const * ids =
		    ent_session_update (session, entities, input->id);
		if (!ids)
		{
			break;
		}

		length_xy * poss =
		    ent_session_update (session, entities, input->pos);
		if (!poss)
		{
			break;
		}

		uint8_t * deads =
		    ent_session_update (session, entities, input->dead);
		if (!deads)
		{
			break;
		}

		size_t len = ent_session_table_len (session, entities);
		for (size_t i = 0; i < len; ++i)
		{
			if (types[i] == type && ids[i] == id)
			{
				memcpy (
				    poss[i],
				    pos,
				    sizeof (length_xy));

				deads[i] = 1;
			}
		}

		rc = ent_session_commit (session);
	}
	while (0);

	ent_session_free (session);
	return rc;
}
