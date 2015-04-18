#include "ent_test.h"

int
lock_invalid_argument_sets_einval (void)
{
	struct ent_column * column = ent_column_alloc (sizeof (int));
	if (!column)
	{
		return -1;
	}

	// Create a table to use a place holder when we want to verify EINVAL
	// for a different parameter.
	struct ent_table * table = ent_table_alloc();
	if (!table)
	{
		ent_column_free (column);
		return -1;
	}

	struct ent_lock * lock = ent_lock_alloc();
	if (!lock)
	{
		ent_column_free (column);
		ent_table_free (table);
		return -1;
	}

	errno = 0;
	assert (ent_lock_table (lock, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_insert (lock, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_insert (NULL, table) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_delete (lock, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_delete (NULL, table) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, table, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (lock, NULL, column) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_update (NULL, table, column) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_select (lock, table, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_select (lock, NULL, column) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_lock_for_select (NULL, table, column) == -1);
	assert (errno = EINVAL);

	errno = 0;
	ent_lock_free (NULL);
	assert (errno = EINVAL);

	ent_lock_free (lock);
	ent_table_free (table);
	ent_column_free (column);

	return 0;
}
