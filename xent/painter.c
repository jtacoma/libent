#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <ent.h>
#include <GL/gl.h>

#include "xent.h"

struct painter
{
	struct ent_table * entities;
	struct ent_processor * processor;
	int pos; // length_xy
};

struct painter *
painter_alloc (
    struct ent_table * entities)
{
	if (!entities)
	{
		errno = EINVAL;
		return NULL;
	}

	struct painter * painter = calloc (1, sizeof (struct painter));
	if (!painter)
	{
		return NULL;
	}

	painter->processor = ent_processor_alloc();
	if (!painter->processor)
	{
		free (painter);
		return NULL;
	}

	painter->pos =
	    ent_processor_use_column (
	        painter->processor, entities, "pos", sizeof (length_xy), "");
	if (painter->pos == -1)
	{
		ent_processor_free (painter->processor);
		free (painter);
		return NULL;
	}

	painter->entities = entities;

	return painter;
}

void
painter_free (
    struct painter * painter)
{
	if (painter)
	{
		if (painter->processor)
		{
			ent_processor_free (painter->processor);
		}

		free (painter);
	}
}

int
painter_paint (
    struct painter * painter,
    length_xy size)
{
	if (!painter)
	{
		errno = EINVAL;
		return -1;
	}

	struct ent_session * session = ent_session_alloc (painter->processor);
	if (session == NULL)
	{
		return -1;
	}

	printf ("visual: %.3fm x %.3fm\n", size[0], size[1]);

	glClearColor (0.0, 0.0, 0.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	struct ent_table * entities = painter->entities;
	length_xy const * poss = (length_xy const *)ent_session_column_get_const (session, entities, painter->pos);
	size_t len = ent_session_table_len (session, entities);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("entity at %.3fm, %.3fm\n", poss[i][0], poss[i][1]);
	}

	ent_session_free (session);

	return 0;
}
