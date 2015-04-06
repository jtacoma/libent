#include "test/ent_test.h"

static void
session_supports_deletion()
{
	struct ent_table * table = ent_table_alloc();
	assert (table);
	struct ent_processor * processor = ent_processor_alloc();
	assert (processor);
	assert (ent_processor_use_table (processor, table, "w") == 0);

	int column_id =
	    ent_processor_use_column (
	        processor, table, "column", sizeof (int), "w");

	struct ent_session * creating = ent_session_alloc (processor);
	struct ent_table * new_table = ent_session_table_insert (creating, table, 8);
	int * new_ints = ent_session_column_get (creating, new_table, column_id);
	for (int i = 0; i < 8; ++i)
	{
		new_ints[i] = i;
	}
	ent_session_commit (creating);
	ent_session_free (creating);
	ent_table_free (new_table);

	struct ent_session * deleting = ent_session_alloc (processor);
	struct ent_rlist * rlist = ent_rlist_alloc();
	ent_rlist_append (rlist, 2, 6);
	ent_session_table_delete (deleting, table, rlist);
	ent_rlist_free (rlist);
	ent_session_commit (deleting);
	ent_session_free (deleting);

	struct ent_session * checking = ent_session_alloc (processor);
	assert (ent_session_table_len (checking, table) == 4);
	int * ints = ent_session_column_get (checking, table, column_id);
	assert (ints[0] == 0);
	assert (ints[1] == 1);
	assert (ints[2] == 6);
	assert (ints[3] == 7);
	ent_session_free (checking);

	ent_processor_free (processor);
}

static void
invalid_argument_sets_errno()
{
	struct ent_table * table = ent_table_alloc();
	assert (table);
	struct ent_processor * processor = ent_processor_alloc();
	assert (processor);

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

	struct ent_session * session = ent_session_alloc (processor);
	assert (session);

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
	assert (ent_session_table_len (session, NULL) == 0);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_session_table_len (NULL, table) == 0);
	assert (errno = EINVAL);

	errno = 0;
	ent_session_free (NULL);
	assert (errno = EINVAL);

	errno = 0;
	ent_processor_free (NULL);
	assert (errno = EINVAL);

	ent_session_free (session);
	ent_processor_free (processor);
	ent_table_free (table);
}

static int
session_general_test()
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
			return -1;
		}

		if (ent_processor_use_table (load, items, "w") == -1)
		{
			return -1;
		}

		int column_b = ent_processor_use_column (load, items, "b", 8, "w");

		if (column_b == -1)
		{
			return -1;
		}

		struct ent_session * loading = ent_session_alloc (load);
		if (!loading)
		{
			return -1;
		}

		struct ent_table * new_items = ent_session_table_insert (loading, items, 2);
		if (!new_items)
		{
			return -1;
		}

		double * b = ent_session_column_get (loading, new_items, column_b);
		if (!b)
		{
			return -1;
		}

		b[0] = 42;
		b[1] = 43;

		ent_table_free (new_items);

		if (ent_session_commit (loading) == -1)
		{
			// TODO: verify that no changes were made
			return -1;
		}

		ent_session_free (loading);
		ent_processor_free (load);
	}

	{
		struct ent_processor * check = ent_processor_alloc();

		if (!check)
		{
			return -1;
		}

		int column_b = ent_processor_use_column (check, items, "b", 8, "w");
		if (column_b == -1)
		{
			return -1;
		}

		struct ent_session * checking = ent_session_alloc (check);

		if (!checking)
		{
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

	return 0;
}

void session_test()
{
	invalid_argument_sets_errno();
	session_supports_deletion();

	int (* functions[])() =
	{
		session_general_test,
	};

	for (size_t f = 0; f < sizeof (functions) / sizeof (*functions); ++f)
	{
		size_t zero = ent_alloc_count();
		assert (functions[f]() == 0);
		size_t used = ent_alloc_count() - zero;

		for (size_t m = 1; m <= used; ++m)
		{
			ent_alloc_artificial_fail (ent_alloc_count() + m);
			errno = 0;
			assert (functions[f]() == -1);
			assert (errno == ENOMEM);
		}
	}
}
