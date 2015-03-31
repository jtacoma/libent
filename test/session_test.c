#include "test/ent_test.h"
#include "table.h"

void session_test()
{
	assert_true (ent_session_alloc (NULL) == NULL);

	struct ent_model * model = ent_model_alloc();
	assert_true (model != NULL);

	struct ent_session * s = ent_session_alloc (model);
	assert_true (s != NULL);

	int items = ent_session_table (s, "items", "w");
	assert_true (items >= 0);
	assert_true (ent_session_table (s, NULL, "w") == -1);
	assert_true (ent_session_table (NULL, "items", "w") == -1);

	assert_true (ent_session_column (s, items, "a", 0, "r") == -1);
	assert_true (ent_session_column (s, items, NULL, 1, "r") == -1);
	assert_true (ent_session_column (s, -1, "a", 1, "r") == -1);
	assert_true (ent_session_column (NULL, items, "a", 1, "r") == -1);

	assert_true (ent_session_column (s, items, "b", 0, "w") == -1);
	assert_true (ent_session_column (s, items, NULL, 8, "w") == -1);
	assert_true (ent_session_column (s, -1, "b", 8, "w") == -1);
	assert_true (ent_session_column (NULL, items, "b", 8, "w") == -1);
	int column_b = ent_session_column (s, items, "b", 8, "w");
	assert_true (column_b >= 0);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_session_lock (NULL) == -1);
	assert_true (ent_session_table_len (s, items) == 0);
	assert_true (ent_session_table_len (s, -1) == 0);
	assert_true (ent_session_table_len (NULL, items) == 0);
	assert_true (ent_session_table_insert (s, items, 0) == -1);
	assert_true (ent_session_table_insert (s, -1, 2) == -1);
	assert_true (ent_session_table_insert (NULL, items, 2) == -1);
	int new_items = ent_session_table_insert (s, items, 2);
	assert_true (new_items >= 0);

	double * b = ent_session_column_write (s, new_items, column_b);
	assert_true (b);
	b[0] = 42;
	b[1] = 43;
	ent_session_free (s);

	s = ent_session_alloc (model);
	assert_true (s != NULL);
	items = ent_session_table (s, "items", "w");
	assert_true (items >= 0);
	assert_true (ent_session_column (s, items, NULL, 8, "w") == -1);
	assert_true (ent_session_column (s, -1, "b", 8, "w") == -1);
	assert_true (ent_session_column (NULL, items, "b", 8, "w") == -1);
	column_b = ent_session_column (s, items, "b", 8, "w");
	assert_true (column_b != -1);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_session_table_len (s, items) == 2);

	b = ent_session_column_write (s, items, column_b);
	assert_true (b);
	assert_true (b[0] == 42);
	assert_true (b[1] == 43);

	ent_session_free (s);
	ent_session_free (NULL);
	ent_model_free (model);
}
