#include "ent_test.h"

int
session_supports_deletion (void)
{
	struct ent_processor * processor = ent_processor_alloc();

	if (!processor)
	{
		return -1;
	}

	struct ent_table * table = ent_table_alloc();

	if (!table)
	{
		ent_processor_free (processor);
		return -1;
	}

	if (ent_processor_use_table (processor, table, "w") == -1)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	int column_id =
	    ent_processor_use_column (
	        processor, table, "column", sizeof (int), "w");

	if (column_id == -1)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	struct ent_session * creating = ent_session_alloc (processor);

	if (!creating)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	struct ent_table * new_table = ent_session_table_insert (creating, table, 8);

	if (!new_table)
	{
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	int * new_ints = ent_session_column_get (creating, new_table, column_id);

	if (!new_ints)
	{
		ent_table_free (new_table);
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	for (int i = 0; i < 8; ++i)
	{
		new_ints[i] = i;
	}

	if (ent_session_commit (creating) == -1)
	{
		ent_table_free (new_table);
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	ent_session_free (creating);
	ent_table_free (new_table);

	struct ent_session * deleting = ent_session_alloc (processor);

	if (!deleting)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	struct ent_rlist * rlist = ent_rlist_alloc();

	if (!rlist)
	{
		ent_session_free (deleting);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	if (ent_rlist_append (rlist, 2, 6) == -1)
	{
		ent_rlist_free (rlist);
		ent_session_free (deleting);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	if (ent_session_table_delete (deleting, table, rlist) == -1)
	{
		ent_rlist_free (rlist);
		ent_session_free (deleting);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	ent_rlist_free (rlist);
	if (ent_session_commit (deleting) == -1)
	{
		ent_session_free (deleting);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}
	ent_session_free (deleting);

	struct ent_session * checking = ent_session_alloc (processor);
	if (!checking)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	assert (ent_session_table_len (checking, table) == 4);

	int * ints = ent_session_column_get (checking, table, column_id);
	assert (ints);
	assert (ints[0] == 0);
	assert (ints[1] == 1);
	assert (ints[2] == 6);
	assert (ints[3] == 7);

	ent_session_free (checking);
	ent_table_free (table);
	ent_processor_free (processor);
	return 0;
}

int
session_supports_insertion (void)
{
	struct ent_table * table = ent_table_alloc();

	if (!table)
	{
		return -1;
	}

	struct ent_processor * processor = ent_processor_alloc();

	if (!processor)
	{
		ent_table_free (table);
		return -1;
	}

	if (ent_processor_use_table (processor, table, "w") == -1)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	int column_id =
	    ent_processor_use_column (
	        processor, table, "column", sizeof (int), "w");

	if (column_id == -1)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	struct ent_session * creating = ent_session_alloc (processor);

	if (!creating)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	struct ent_table * new_table = ent_session_table_insert (creating, table, 16);

	if (!new_table)
	{
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	int * new_ints = ent_session_column_get (creating, new_table, column_id);

	if (!new_ints)
	{
		ent_table_free (new_table);
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	for (int i = 0; i < 8; ++i)
	{
		new_ints[i] = i;
	}

	if (ent_session_commit (creating) == -1)
	{
		ent_table_free (new_table);
		ent_session_free (creating);
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	ent_session_free (creating);
	ent_table_free (new_table);

	struct ent_session * checking = ent_session_alloc (processor);
	if (!checking)
	{
		ent_table_free (table);
		ent_processor_free (processor);
		return -1;
	}

	assert (ent_session_table_len (checking, table) == 16);

	int * ints = ent_session_column_get (checking, table, column_id);
	assert (ints);
	assert (ints[0] == 0);
	assert (ints[1] == 1);
	assert (ints[2] == 2);
	assert (ints[3] == 3);
	assert (ints[4] == 4);
	assert (ints[5] == 5);
	assert (ints[6] == 6);
	assert (ints[7] == 7);

	ent_session_free (checking);
	ent_table_free (table);
	ent_processor_free (processor);
	return 0;
}

int
session_invalid_argument_sets_einval (void)
{
	struct ent_table * table = ent_table_alloc();
	if (!table)
	{
		return -1;
	}

	struct ent_processor * processor = ent_processor_alloc();
	if (!processor)
	{
		ent_table_free (table);
		return -1;
	}

	struct ent_rlist * rlist = ent_rlist_alloc();
	if (!rlist)
	{
		ent_processor_free (processor);
		ent_table_free (table);
		return -1;
	}

	errno = 0;
	assert (ent_session_alloc (NULL) == NULL);
	assert (errno = EINVAL);

	struct ent_session * session = ent_session_alloc (processor);
	if (!session)
	{
		ent_rlist_free (rlist);
		ent_processor_free (processor);
		ent_table_free (table);
		return -1;
	}

	errno = 0;
	assert (ent_session_table_insert (session, table, 0) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_insert (session, NULL, 2) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_insert (NULL, table, 2) == NULL);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_delete (session, table, NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_delete (session, NULL, rlist) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_delete (NULL, table, rlist) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_len (session, NULL) == 0);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_len (NULL, table) == 0);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_commit (NULL) == -1);
	assert (errno = EINVAL);

	errno = 0;
	ent_session_free (NULL);
	assert (errno = EINVAL);

	errno = 0;
	ent_processor_free (NULL);
	assert (errno = EINVAL);

	ent_session_free (session);
	ent_rlist_free (rlist);
	ent_processor_free (processor);
	ent_table_free (table);

	return 0;
}

int
session_general_test (void)
{
	struct ent_table * items = ent_table_alloc();

	if (!items)
	{
		return -1;
	}

	{
		struct ent_processor * load = ent_processor_alloc();

		if (!load)
		{
			ent_table_free (items);
			return -1;
		}

		if (ent_processor_use_table (load, items, "w") == -1)
		{
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		int column_b = ent_processor_use_column (load, items, "b", 8, "w");

		if (column_b == -1)
		{
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		struct ent_session * loading = ent_session_alloc (load);

		if (!loading)
		{
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		struct ent_table * new_items = ent_session_table_insert (loading, items, 2);

		if (!new_items)
		{
			ent_session_free (loading);
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		double * b = ent_session_column_get (loading, new_items, column_b);

		if (!b)
		{
			ent_table_free (new_items);
			ent_session_free (loading);
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		b[0] = 42;
		b[1] = 43;

		ent_table_free (new_items);

		if (ent_session_commit (loading) == -1)
		{
			// TODO: verify that no changes were made
			ent_session_free (loading);
			ent_processor_free (load);
			ent_table_free (items);
			return -1;
		}

		ent_session_free (loading);
		ent_processor_free (load);
	}

	{
		struct ent_processor * check = ent_processor_alloc();

		if (!check)
		{
			ent_table_free (items);
			return -1;
		}

		int column_b = ent_processor_use_column (check, items, "b", 8, "w");
		if (column_b == -1)
		{
			ent_processor_free (check);
			ent_table_free (items);
			return -1;
		}

		struct ent_session * checking = ent_session_alloc (check);

		if (!checking)
		{
			ent_processor_free (check);
			ent_table_free (items);
			return -1;
		}

		assert (ent_session_table_len (checking, items) == 2);

		double const * b = ent_session_column_get_const (checking, items, column_b);
		assert (b);
		assert (b[0] == 42);
		assert (b[1] == 43);

		ent_session_free (checking);
		ent_processor_free (check);
	}

	ent_table_free (items);

	return 0;
}
