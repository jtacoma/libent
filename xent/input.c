#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ent.h>

#include "xent.h"

struct input
{
	struct ent_table * entities;
	struct ent_processor * processor;
	int type;
	int id;
	int action;
	int pos;
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

	input->processor = ent_processor_alloc();
	if (!input->processor)
	{
		free (input);
		return NULL;
	}

	input->type =
	    ent_processor_use_column (
	        input->processor, entities, "input_type", sizeof (enum input_type), "");
	if (input->type == -1)
	{
		ent_processor_free (input->processor);
		free (input);
		return NULL;
	}

	input->id =
	    ent_processor_use_column (
	        input->processor, entities, "input_id", sizeof (input_id), "");
	if (input->id == -1)
	{
		ent_processor_free (input->processor);
		free (input);
		return NULL;
	}

	input->action =
	    ent_processor_use_column (
	        input->processor, entities, "input_action", sizeof (enum input_action), "");
	if (input->action == -1)
	{
		ent_processor_free (input->processor);
		free (input);
		return NULL;
	}

	input->pos =
	    ent_processor_use_column (
	        input->processor, entities, "pos", sizeof (length_xy), "");
	if (input->pos == -1)
	{
		ent_processor_free (input->processor);
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
input_append (
    struct input * input,
    enum input_type type,
    input_id id,
    enum input_action action,
    length_xy pos)
{
	int rc = -1;

	struct ent_session * session =
	    ent_session_alloc (input->processor);
	if (!session)
	{
		return rc;
	}

	do
	{
		printf ("input!\n");

		struct ent_table * one =
		    ent_session_table_insert (session, input->entities, 1);
		if (!one)
		{
			break;
		}

		enum input_type * types =
		    ent_session_column_get (session, one, input->type);
		if (!types)
		{
			break;
		}
		types[0] = type;

		input_id * ids =
		    ent_session_column_get (session, one, input->id);
		if (!ids)
		{
			break;
		}
		ids[0] = id;

		enum input_action * actions =
		    ent_session_column_get (session, one, input->action);
		if (!actions)
		{
			break;
		}
		actions[0] = action;

		length_xy * poss =
		    ent_session_column_get (session, one, input->pos);
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
