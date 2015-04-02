#include "ent.h"

#include <assert.h>
#include <stdio.h>

static struct
{
	int entities;
	int column_mass;
} build_info;

int build_model (struct ent_session * s, void * info)
{
	(void)info;

	// Append some items to the "entities" table.  Data in all columns
	// defaults to zero.
	const size_t appending = 9;
	int new_entities = ent_session_table_insert (s, build_info.entities, appending);
	assert (new_entities >= 0);

	// Get a non-const pointer to the appended "mass" data.
	double * masses = ent_session_column_write (s, new_entities, build_info.column_mass);
	assert (masses);

	// This array of float64 values can be used as normal.
	for (size_t i = 0; i < appending; ++i)
	{
		masses[i] = (double) (i * i) / 3;
	}

	return 0;
}

static struct
{
	int entities;
	int column_mass;
} print_info;

int print_model (struct ent_session * s, void * info)
{
	(void)info;

	size_t len = ent_session_table_len (s, print_info.entities);
	double const * masses = ent_session_column_read (s, print_info.entities, print_info.column_mass);
	assert (masses);

	for (size_t i = 0; i < len; ++i)
	{
		printf ("masses[%lu] == %0.1f\n", i, masses[i]);
	}

	return 0;
}

static void load (struct ent_model * model)
{
	// Create a processor through which to build the model.
	struct ent_processor * processor = ent_processor_alloc (model);
	assert (processor);

	// Create a table (or get one if it already exists).
	build_info.entities = ent_processor_use_table (processor, "entities", "w");
	assert (build_info.entities >= 0);

	// Declare that we want to set the "mass" on items appended to the
	// "entities" table.
	build_info.column_mass = ent_processor_use_column (processor, build_info.entities, "mass", 8, "w");
	assert (build_info.column_mass >= 0);

	ent_processor_set_function (processor, build_model, NULL);

	ent_model_set_processor (model, "build", processor);

	ent_processor_free (processor);

	// Allocate a new session in which we will verify that the model has
	// been changed as described above.  The steps are very similar.
	processor = ent_processor_alloc (model);
	assert (processor);

	print_info.entities = ent_processor_use_table (processor, "entities", "r");
	assert (print_info.entities >= 0);

	// This time we use the "_r" variant because we're only interested in
	// reading existing data.
	print_info.column_mass = ent_processor_use_column (processor, print_info.entities, "mass", 8, "r");
	assert (print_info.column_mass >= 0);

	ent_processor_set_function (processor, print_model, NULL);

	ent_model_set_processor (model, "print", processor);

	ent_processor_free (processor);
}

int main()
{
	// Create a model that will hold all tables.
	struct ent_model * model = ent_model_alloc();
	assert (model);

	ent_model_invoke ("build");
	ent_model_invoke ("print");

	ent_model_free (model);
}
