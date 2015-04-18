#include "ent_test.h"

int
lock_invalid_argument_sets_einval (void)
{
	// Create a table to use a place holder when we want to verify EINVAL
	// for a different parameter.
	struct ent_table * table = ent_table_alloc();
	if (!table)
	{
		return -1;
	}

	errno = 0;
	assert (ent_lock_table (NULL, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_column (NULL, 0).name == NULL);
	assert (ent_lock_column (NULL, 0).width == 0);
	assert (errno = EINVAL);

	struct ent_lock * lock = ent_lock_alloc();
	if (!lock)
	{
		return -1;
	}

	errno = 0;
	assert (ent_lock_table (lock, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_column (lock, 0).name == NULL);
	assert (ent_lock_column (lock, 0).width == 0);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_insert (lock, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_insert (NULL, table) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, table, "a", 0) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, table, NULL, 1) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, NULL, "a", 1) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (NULL, table, "a", 1) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, table, "b", 0) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, table, NULL, 8) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, NULL, "b", 8) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (NULL, table, "b", 8) == -1);
	assert (errno = EINVAL);

	errno = 0;
	ent_lock_free (NULL);
	assert (errno = EINVAL);

	ent_lock_free (lock);
	ent_table_free (table);

	return 0;
}
