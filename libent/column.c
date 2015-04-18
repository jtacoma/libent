#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "rlist.h"
#include "column.h"

static int last_id = 0;

struct ent_column *
ent_column_alloc (size_t width)
{
	struct ent_column * column = NULL;

	if (!width)
	{
		errno = EINVAL;
	}
	else if (ent_alloc ((void**)&column, sizeof (*column)) == 0)
	{
		*column = (struct ent_column) {0};
		column->width = width;
		column->id = ++last_id;
	}

	return column;
}

void
ent_column_free (
    struct ent_column * column)
{
	if (!column)
	{
		errno = EINVAL;
		return;
	}

	ent_alloc ((void**)&column, 0);
}

int
ent_column_id (
    struct ent_column const * column)
{
	if (!column)
	{
		errno = EINVAL;
		return -1;
	}

	return column->id;
}

size_t
ent_column_width (
    struct ent_column const * column)
{
	if (!column)
	{
		errno = EINVAL;
		return -1;
	}

	return column->width;
}
