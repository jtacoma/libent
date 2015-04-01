#include "ent.h"

#include <assert.h>
#include <stdio.h>

void build_model (struct ent_model * model)
{
	// Create a session through which to build the model.
	struct ent_session * s = ent_session_alloc (model);
	assert (s);

	// Create a table (or get one if it already exists).
	int entities = ent_session_table (s, "entities", "w");
	assert (entities >= 0);

	// Declare that we want to set the "mass" on items appended to the
	// "entities" table.
	int column_mass =
	    ent_session_column (s, entities, "mass", 8, "w");
	assert (column_mass >= 0);

	// Acquire the necessary mutexes etc. and transition to the locked
	// state.
	assert (ent_session_lock (s) == 0);

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	const size_t appending = 9;
	int new_entities = ent_session_table_insert (s, entities, appending);
	assert (new_entities >= 0);

	// Get a non-const pointer to the appended "mass" data.
	double * masses = ent_session_column_write (s, new_entities, column_mass);
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

	int entities = ent_session_table (s, "entities", "r");
	assert (entities >= 0);

	// This time we use the "_r" variant because we're only interested in
	// reading existing data.
	int column_mass = ent_session_column (s, entities, "mass", 8, "r");
	assert (column_mass >= 0);

	assert (ent_session_lock (s) == 0);

	size_t len = ent_session_table_len (s, entities);
	double const * masses = ent_session_column_read (s, entities, column_mass);
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
