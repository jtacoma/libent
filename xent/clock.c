#define _XOPEN_SOURCE 500

#include "clock.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

struct clock
{
	long frame;
	struct timeval time_started;
	unsigned long tick_offset;
};

struct clock * clock_alloc()
{
	return calloc (1, sizeof (struct clock));
}

void clock_free (struct clock * t)
{
	free (t);
}

int
clock_sleep (struct clock * t)
{
	if (!t->time_started.tv_sec)
	{
		gettimeofday (&t->time_started, NULL);
		t->tick_offset = t->time_started.tv_usec * 60 / 1000000;
		return 0;
	}

	t->frame += 1;

	struct timeval tick =
	{
		.tv_sec = t->time_started.tv_sec + (t->tick_offset + t->frame) / 60,
		.tv_usec = ((t->tick_offset + t->frame) % 60) * 1000000 / 60,
	};

	printf ("expected time: %ld.%06lds\n", tick.tv_sec, tick.tv_usec);

	int go_to_sleep = 1;

	while (go_to_sleep)
	{
		struct timeval now;

		int error = gettimeofday (&now, NULL);
		if (error)
		{
			printf ("cannot get time of day: %x\n", errno);
			return -1;
		}

		printf ("actual time: %ld.%06lds\n", now.tv_sec, now.tv_usec);

		long interval = (tick.tv_usec - now.tv_usec)
		                + 1000000 * (tick.tv_sec - now.tv_sec);

		printf ("idle: %.2f%%\n", 100 * (float)interval / (1000000 / 60));

		if (interval < 0)
		{
			return 0;
		}

		if (interval > 1000000 / 60)
		{
			interval = 1000000 / 60;
		}

		if (usleep ((useconds_t)interval))
		{
			if (errno == EINVAL)
			{
				printf ("logic error: nsec too large\n");
				return -1;
			}
			else if (errno == EINTR)
			{
				break;
			}
		}
		else
		{
			go_to_sleep = 0;
		}
	}

	return 0;
}
