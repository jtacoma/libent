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
	struct ent_lock * lock;
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

	game->lock = ent_lock_alloc();
	if (!game->lock)
	{
		free (game);
		return NULL;
	}

	game->pos =
	    ent_lock_for_update (
	        game->lock, entities, pos_column);
	if (game->pos == -1)
	{
		ent_lock_free (game->lock);
		free (game);
		return NULL;
	}

	game->age =
	    ent_lock_for_update (
	        game->lock, entities, age_column);
	if (game->age == -1)
	{
		ent_lock_free (game->lock);
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
		if (game->lock)
		{
			ent_lock_free (game->lock);
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

	struct ent_session * session = ent_session_alloc (game->lock);
	if (session == NULL)
	{
		return -1;
	}

	struct ent_table * entities = game->entities;

	do
	{
		size_t len = ent_session_table_len (session, entities);
		float * ages = ent_session_update (session, entities, age_column);

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
