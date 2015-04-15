#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <ent.h>
#include <GL/gl.h>

#include "xent.h"
#include "game.h"

struct game
{
	struct ent_table * entities;
	struct ent_processor * processor;
	int pos; // length_xy
	int age; // float
};

struct game *
game_alloc (
    struct ent_table * entities)
{
	if (!entities)
	{
		errno = EINVAL;
		return NULL;
	}

	struct game * game = calloc (1, sizeof (struct game));
	if (!game)
	{
		return NULL;
	}

	game->processor = ent_processor_alloc();
	if (!game->processor)
	{
		free (game);
		return NULL;
	}

	game->pos =
	    ent_processor_use_column (
	        game->processor, entities, "pos", sizeof (length_xy), "");
	if (game->pos == -1)
	{
		ent_processor_free (game->processor);
		free (game);
		return NULL;
	}

	game->age =
	    ent_processor_use_column (
	        game->processor, entities, "age", sizeof (float), "");
	if (game->age == -1)
	{
		ent_processor_free (game->processor);
		free (game);
		return NULL;
	}

	game->entities = entities;

	return game;
}

void
game_free (
    struct game * game)
{
	if (game)
	{
		if (game->processor)
		{
			ent_processor_free (game->processor);
		}

		free (game);
	}
}

int
game_logic (
    struct game * game)
{
	if (!game)
	{
		errno = EINVAL;
		return -1;
	}

	struct ent_session * session = ent_session_alloc (game->processor);
	if (session == NULL)
	{
		return -1;
	}

	struct ent_table * entities = game->entities;

	do
	{
		size_t len = ent_session_table_len (session, entities);
		float * ages = ent_session_column_get (session, entities, game->age);

		for (size_t i = 0; i < len; ++i)
		{
			ages[i] += 1;
		}

		break;
	}
	while (0);

	ent_session_free (session);

	return 0;
}
