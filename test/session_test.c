#include "test/ent_test.h"
#include "table.h"

void session_test()
{
	assert_true (ent_processor_alloc (NULL) == NULL);

	struct ent_model * model = ent_model_alloc();
	assert_true (model != NULL);

	{
		struct ent_processor * load = ent_processor_alloc (model);
		assert_true (load != NULL);

		int items = ent_processor_use_table (load, "items", "w");
		assert_true (items >= 0);
		assert_true (ent_processor_use_table (load, NULL, "w") == -1);
		assert_true (ent_processor_use_table (NULL, "items", "w") == -1);

		assert_true (ent_processor_use_column (load, items, "a", 0, "r") == -1);
		assert_true (ent_processor_use_column (load, items, NULL, 1, "r") == -1);
		assert_true (ent_processor_use_column (load, -1, "a", 1, "r") == -1);
		assert_true (ent_processor_use_column (NULL, items, "a", 1, "r") == -1);

		assert_true (ent_processor_use_column (load, items, "b", 0, "w") == -1);
		assert_true (ent_processor_use_column (load, items, NULL, 8, "w") == -1);
		assert_true (ent_processor_use_column (load, -1, "b", 8, "w") == -1);
		assert_true (ent_processor_use_column (NULL, items, "b", 8, "w") == -1);

		int column_b = ent_processor_use_column (load, items, "b", 8, "w");
		assert_true (column_b >= 0);

		struct ent_session * loading = ent_session_alloc (load);
		assert_true (loading);

		assert_true (ent_session_table_len (loading, items) == 0);
		assert_true (ent_session_table_len (loading, -1) == 0);
		assert_true (ent_session_table_len (NULL, items) == 0);

		assert_true (ent_session_table_insert (loading, items, 0) == -1);
		assert_true (ent_session_table_insert (loading, -1, 2) == -1);
		assert_true (ent_session_table_insert (NULL, items, 2) == -1);

		int new_items = ent_session_table_insert (loading, items, 2);
		assert_true (new_items >= 0);

		double * b = ent_session_column_write (loading, new_items, column_b);
		assert_true (b);
		b[0] = 42;
		b[1] = 43;

		assert_true (ent_session_commit (loading) == 0);
		ent_session_free (loading);
		ent_processor_free (load);
		ent_processor_free (NULL);
	}

	{
		struct ent_processor * check = ent_processor_alloc (model);
		assert_true (check != NULL);
		int items = ent_processor_use_table (check, "items", "w");
		assert_true (items >= 0);
		assert_true (ent_processor_use_column (check, items, NULL, 8, "w") == -1);
		assert_true (ent_processor_use_column (check, -1, "b", 8, "w") == -1);
		assert_true (ent_processor_use_column (NULL, items, "b", 8, "w") == -1);
		int column_b = ent_processor_use_column (check, items, "b", 8, "w");
		assert_true (column_b != -1);

		struct ent_session * checking = ent_session_alloc (check);

		assert_true (ent_session_table_len (checking, items) == 2);

		double * b = ent_session_column_write (checking, items, column_b);
		assert_true (b);
		assert_true (b[0] == 42);
		assert_true (b[1] == 43);

		ent_session_free (checking);
		ent_processor_free (check);
	}

	ent_model_free (model);
}
