#include "ent_test.h"

int
processor_invalid_argument_sets_einval (void)
{
	// Create a table to use a place holder when we want to verify EINVAL
	// for a different parameter.
	struct ent_table * table = ent_table_alloc();
	if (!table)
	{
		return -1;
	}

	errno = 0;
	assert (ent_processor_table (NULL, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_column (NULL, 0).name == NULL);
	assert (ent_processor_column (NULL, 0).width == 0);
	assert (errno = EINVAL);

	struct ent_processor * processor = ent_processor_alloc();
	if (!processor)
	{
		return -1;
	}

	errno = 0;
	assert (ent_processor_table (processor, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_column (processor, 0).name == NULL);
	assert (ent_processor_column (processor, 0).width == 0);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_table (processor, NULL, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_table (NULL, table, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, table, "a", 0, "r") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, table, NULL, 1, "r") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, NULL, "a", 1, "r") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (NULL, table, "a", 1, "r") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, table, "b", 0, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, table, NULL, 8, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (processor, NULL, "b", 8, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_processor_use_column (NULL, table, "b", 8, "w") == -1);
	assert (errno = EINVAL);

	errno = 0;
	ent_processor_free (NULL);
	assert (errno = EINVAL);

	ent_processor_free (processor);
	ent_table_free (table);

	return 0;
}
