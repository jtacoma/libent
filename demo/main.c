#include "ent.h"

#include <assert.h>
#include <stdio.h>

void build_model (struct ent_model * model)
{
	// Create a session through which to build the model.
	struct ent_session * s = ent_session_alloc (model);
	assert (s);

	// Create a table (or get one if it already exists).
	struct ent_table * entities = ent_session_table (s, "entities");
	assert (entities);

	// Declare that we want to set the "mass" on items appended to the
	// "entities" table.
	struct ent_column * column_mass =
	    ent_session_column_a (s, entities, "mass", 8);
	assert (column_mass);

	// Acquire the necessary mutexes etc. and transition to the locked
	// state.
	assert (ent_session_lock (s) == 0);

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	const size_t appending = 9;
	assert (ent_session_table_grow (s, entities, appending) == 0);

	// Get a non-const pointer to the appended "mass" data.
	double * masses = ent_column_ref (column_mass);
	assert (masses);

	// This array of float64 values can be used as normal.
	for (size_t i = 0; i < appending; ++i)
	{
		masses[i] = (double) (i * i) / 3;
	}

	// Free the session.  This also invalidates all pointers received from
	// it e.g. masses, column_masses, and entities.
	ent_session_free (s);
}

void print_model (struct ent_model * model)
{
	// Allocate a new session in which we will verify that the model has
	// been changed as described above.  The steps are very similar.
	struct ent_session * s = ent_session_alloc (model);
	assert (s);

	struct ent_table * entities = ent_session_table (s, "entities");
	assert (entities);

	// This time we use the "_r" variant because we're only interested in
	// reading existing data.
	struct ent_column const * column_mass = ent_session_column_r (s, entities, "mass", 8);
	assert (column_mass != NULL);

	assert (ent_session_lock (s) == 0);

	size_t len = ent_session_table_len (s, entities);
	double const * masses = ent_column_get (column_mass);
	assert (masses);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("masses[%lu] == %0.1f\n", i, masses[i]);
	}

	// Always remember to free the session.  Of course.
	ent_session_free (s);
}

int main()
{
	// Create a model that will hold all tables.
	struct ent_model * model = ent_model_alloc();
	assert (model);

	build_model (model);

	print_model (model);

	ent_model_free (model);
}
