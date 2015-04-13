#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <ent.h>

#include "xent.h"
#include "window.h"
#include "clock.h"

static int
main_loop (
    struct ent_table * entities,
    struct window * w,
    struct clock * c)
{
	if (window_setup (w) == -1)
	{
		return -1;
	}

	unsigned long frame = 0;
	int error = 0;
	int status = 0;

	while (!window_stopping (w))
	{
		error = window_paint (w);
		if (error)
		{
			break;
		}

		error = clock_sleep (c);
		if (error)
		{
			break;
		}

		frame += 1;
		if (! (frame % 60))
		{
			printf ("frame %ld\n", frame);
		}

		error = window_input (w);
		if (error)
		{
			break;
		}

		// TODO: iterate state based on input etc.
	}

	if (window_teardown (w) == -1)
	{
		return -1;
	}

	return (error || status < 0) ? -1 : 0;
}

int
main()
{
	struct ent_table * entities = ent_table_alloc();
	if (!entities)
	{
		return -1;
	}

	struct window * w = window_alloc (entities);
	if (!w)
	{
		ent_table_free (entities);
		return -1;
	}

	struct clock * c = clock_alloc();
	if (!c)
	{
		window_free (w);
		ent_table_free (entities);
		return -1;
	}

	int rc = main_loop (entities, w, c);

	clock_free (c);
	window_free (w);
	ent_table_free (entities);

	return rc;
}
