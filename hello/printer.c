#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include <ent.h>

#include "columns.h"

void print (struct ent_table * entities)
{
	struct ent_lock * printer = ent_lock_alloc();
	assert (printer);

	assert (ent_lock_for_update (printer, entities, mass_column) == 0);

	struct ent_session * printing = ent_session_alloc (printer);
	assert (printing);

	size_t len = ent_session_table_len (printing, entities);
	assert (len > 0);

	double const * masses = ent_session_select (printing, entities, mass_column);
	assert (masses);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("masses[%lu] == %0.1f\n", i, masses[i]);
	}

	ent_session_free (printing);
	ent_lock_free (printer);
}
