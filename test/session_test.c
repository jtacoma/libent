#include "test/ent_test.h"
#include "table.h"

void session_supports_deletion()
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

void session_test()
{
	session_supports_deletion();

	struct ent_table * items = ent_table_alloc();
	assert (items);

	{
		struct ent_processor * load = ent_processor_alloc();
		assert (load != NULL);

		assert (ent_processor_use_table (load, items, "w") == 0);
		assert (ent_processor_use_table (load, NULL, "w") == -1);
		assert (ent_processor_use_table (NULL, items, "w") == -1);

		assert (ent_processor_use_column (load, items, "a", 0, "r") == -1);
		assert (ent_processor_use_column (load, items, NULL, 1, "r") == -1);
		assert (ent_processor_use_column (load, NULL, "a", 1, "r") == -1);
		assert (ent_processor_use_column (NULL, items, "a", 1, "r") == -1);

		assert (ent_processor_use_column (load, items, "b", 0, "w") == -1);
		assert (ent_processor_use_column (load, items, NULL, 8, "w") == -1);
		assert (ent_processor_use_column (load, NULL, "b", 8, "w") == -1);
		assert (ent_processor_use_column (NULL, items, "b", 8, "w") == -1);

		int column_b = ent_processor_use_column (load, items, "b", 8, "w");
		assert (column_b >= 0);

		struct ent_session * loading = ent_session_alloc (load);
		assert (loading);

		assert (ent_session_table_len (loading, items) == 0);
		assert (ent_session_table_len (loading, NULL) == 0);
		assert (ent_session_table_len (NULL, items) == 0);

		assert (ent_session_table_insert (loading, items, 0) == NULL);
		assert (ent_session_table_insert (loading, NULL, 2) == NULL);
		assert (ent_session_table_insert (NULL, items, 2) == NULL);

		struct ent_table * new_items = ent_session_table_insert (loading, items, 2);
		assert (new_items);

		double * b = ent_session_column_get (loading, new_items, column_b);
		assert (b);
		b[0] = 42;
		b[1] = 43;

		ent_table_free (new_items);
		assert (ent_session_commit (loading) == 0);
		ent_session_free (loading);
		ent_processor_free (load);
		ent_processor_free (NULL);
	}

	{
		struct ent_processor * check = ent_processor_alloc();
		assert (check);
		assert (ent_processor_use_column (check, items, NULL, 8, "w") == -1);
		assert (ent_processor_use_column (check, NULL, "b", 8, "w") == -1);
		assert (ent_processor_use_column (NULL, items, "b", 8, "w") == -1);
		int column_b = ent_processor_use_column (check, items, "b", 8, "w");
		assert (column_b != -1);

		struct ent_session * checking = ent_session_alloc (check);

		assert (ent_session_table_len (checking, items) == 2);

		double * b = ent_session_column_get (checking, items, column_b);
		assert (b);
		assert (b[0] == 42);
		assert (b[1] == 43);

		ent_session_free (checking);
		ent_processor_free (check);
	}
}
