#include "ent.h"

#include <assert.h>
#include <stdio.h>

static void build (struct ent_model * model)
{
	// Create a processor through which to build the model.
	struct ent_processor * builder = ent_processor_alloc (model);
	assert (builder);

	// Create a table (or get one if it already exists).
	int entities = ent_processor_use_table (builder, "entities", "w");
	assert (entities >= 0);

	// Declare that we want to set the "mass" on items appended to the
	// "entities" table.
	int column_mass = ent_processor_use_column (builder, entities, "mass", 8, "w");
	assert (column_mass >= 0);

	struct ent_session * building = ent_session_alloc (builder);

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	const size_t appending = 9;
	int new_entities = ent_session_table_insert (building, entities, appending);
	assert (new_entities >= 0);

	// Get a non-const pointer to the appended "mass" data.
	double * masses = ent_session_column_write (building, new_entities, column_mass);
	assert (masses);

	// This array of float64 values can be used as normal.
	for (size_t i = 0; i < appending; ++i)
	{
		masses[i] = (double) (i * i) / 3;
	}

	ent_session_commit (building);
	ent_session_free (building);
	ent_processor_free (builder);
}

static void print (struct ent_model * model)
{
	// Allocate a new session in which we will verify that the model has
	// been changed as described above.  The steps are very similar.
	struct ent_processor * printer = ent_processor_alloc (model);
	assert (printer);

	int entities = ent_processor_use_table (printer, "entities", "r");
	assert (entities >= 0);

	// This time we use the "_r" variant because we're only interested in
	// reading existing data.
	int column_mass = ent_processor_use_column (printer, entities, "mass", 8, "r");
	assert (column_mass >= 0);

	struct ent_session * printing = ent_session_alloc (printer);
	assert (printing);

	size_t len = ent_session_table_len (printing, entities);
	assert (len > 0);

	double const * masses = ent_session_column_read (printing, entities, column_mass);
	assert (masses);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("masses[%lu] == %0.1f\n", i, masses[i]);
	}

	ent_session_free (printing);
	ent_processor_free (printer);
}

int main()
{
	// Create a model that will hold all tables.
	struct ent_model * model = ent_model_alloc();
	assert (model);

	build (model);
	print (model);

	ent_model_free (model);
}
