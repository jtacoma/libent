#include "ent.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

void print (struct ent_model * model)
{
	// Allocate a new session in which we will verify that the model has
	// been changed as described above.  The steps are very similar.
	struct ent_processor * printer = ent_processor_alloc (model);
	assert (printer);

	int entities = ent_processor_use_table (printer, "entities", "r");
	assert (entities >= 0);

	// This time we use the "_r" variant because we're only interested in
	// reading existing data.
	int column_mass = ent_processor_use_column (printer, entities, "mass", sizeof (double), "r");
	assert (column_mass >= 0);

	struct ent_session * printing = ent_session_alloc (printer);
	assert (printing);

	size_t len = ent_session_table_len (printing, entities);
	assert (len > 0);

	double const * masses = ent_session_column_get (printing, entities, column_mass);
	assert (masses);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("masses[%lu] == %0.1f\n", i, masses[i]);
	}

	ent_session_free (printing);
	ent_processor_free (printer);
}
