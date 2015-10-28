#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "columns.h"

struct ent_column * mass_column;
struct ent_column * age_column;

static bool  initialized = false;

void columns_init ()
{
	if (!initialized)
	{
		mass_column = ent_column_alloc (sizeof (mass));
		age_column = ent_column_alloc (sizeof (age));
		initialized = true;
	}
}
