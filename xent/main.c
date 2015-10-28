#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <ent.h>

#include "xent.h"
#include "window.h"
#include "clock.h"
#include "game.h"
#include "painter.h"

static int
main_loop (
    struct ent_table * entities,
    struct window * window,
    struct clock * clock,
    struct game * game,
    struct painter * painter)
{
	if (window_setup (window) == -1)
	{
		perror ("window_setup");
		return -1;
	}

	unsigned long frame = 0;
	int error = 0;
	int status = 0;

	while (!window_stopping (window))
	{
		error = window_paint_begin (window);
		if (error)
		{
			perror ("window_paint_begin");
			break;
		}

		length_xy visual_size;
		window_visual_size (window, visual_size);
		error = painter_paint (painter, visual_size);
		if (error)
		{
			perror ("painter_paint");
			break;
		}

		error = window_paint_end (window);
		if (error)
		{
			perror ("window_paint_end");
			break;
		}

		error = clock_sleep (clock);
		if (error)
		{
			perror ("clock_sleep");
			break;
		}

		frame += 1;
		if (! (frame % 60))
		{
			printf ("frame %ld\n", frame);
		}

		error = window_input (window);
		if (error)
		{
			perror ("window_input");
			break;
		}

		error = game_logic (game);
		if (error)
		{
			perror ("game_logic");
			break;
		}
	}

	if (window_teardown (window) == -1)
	{
		perror ("window_teardown");
		return -1;
	}

	return (error || status < 0) ? -1 : 0;
}

int
main ()
{
	xent_init_columns ();

	struct ent_table * entities = ent_table_alloc ();
	if (!entities)
	{
		perror ("ent_table_alloc");
		return -1;
	}

	struct window * window = window_alloc (entities);
	if (!window)
	{
		perror ("window_alloc");
		ent_table_free (entities);
		return -1;
	}

	struct clock * clock = clock_alloc ();
	if (!clock)
	{
		perror ("clock_alloc");
		window_free (window);
		ent_table_free (entities);
		return -1;
	}

	struct game * game = game_alloc (entities);
	if (!game)
	{
		perror ("game_alloc");
		clock_free (clock);
		window_free (window);
		ent_table_free (entities);
		return -1;
	}

	struct painter * painter = painter_alloc (entities);
	if (!painter)
	{
		perror ("painter_alloc");
		game_free (game);
		clock_free (clock);
		window_free (window);
		ent_table_free (entities);
		return -1;
	}

	int rc = main_loop (entities, window, clock, game, painter);

	clock_free (clock);
	window_free (window);
	ent_table_free (entities);

	return rc;
}
