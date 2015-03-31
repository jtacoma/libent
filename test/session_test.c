#include "test/ent_test.h"
#include "table.h"

void session_test()
{
	assert_true (ent_session_alloc (NULL) == NULL);

	struct ent_model * model = ent_model_alloc();
	assert_true (model != NULL);

	struct ent_session * s = ent_session_alloc (model);
	assert_true (s != NULL);

	struct ent_table * items = ent_session_table (s, "items");
	assert_true (items != NULL);
	assert_true (ent_session_table (s, NULL) == NULL);
	assert_true (ent_session_table (NULL, "items") == NULL);

	assert_true (ent_session_column_r (s, items, "a", 0) == NULL);
	assert_true (ent_session_column_r (s, items, NULL, 1) == NULL);
	assert_true (ent_session_column_r (s, NULL, "a", 1) == NULL);
	assert_true (ent_session_column_r (NULL, items, "a", 1) == NULL);

	assert_true (ent_session_column_a (s, items, "b", 0) == NULL);
	assert_true (ent_session_column_a (s, items, NULL, 8) == NULL);
	assert_true (ent_session_column_a (s, NULL, "b", 8) == NULL);
	assert_true (ent_session_column_a (NULL, items, "b", 8) == NULL);
	struct ent_column * column_b = ent_session_column_a (s, items, "b", 8);
	assert_true (column_b != NULL);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_session_lock (NULL) == -1);
	assert_true (ent_session_table_len (s, items) == 0);
	assert_true (ent_session_table_len (s, NULL) == 0);
	assert_true (ent_session_table_len (NULL, items) == 0);
	assert_true (ent_session_table_grow (s, items, 0) == -1);
	assert_true (ent_session_table_grow (s, items, 2) == 0);
	assert_true (ent_session_table_grow (s, NULL, 2) == -1);
	assert_true (ent_session_table_grow (NULL, items, 2) == -1);

	double * b = ent_column_ref (column_b);
	assert_true (b);
	b[0] = 42;
	b[1] = 43;
	ent_session_free (s);

	s = ent_session_alloc (model);
	assert_true (s != NULL);
	items = ent_session_table (s, "items");
	assert_true (items != NULL);
	assert_true (ent_session_column_w (s, items, NULL, 8) == NULL);
	assert_true (ent_session_column_w (s, NULL, "b", 8) == NULL);
	assert_true (ent_session_column_w (NULL, items, "b", 8) == NULL);
	column_b = ent_session_column_w (s, items, "b", 8);
	assert_true (column_b != NULL);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_table_len (items) == 2);

	b = ent_column_ref (column_b);
	assert_true (b);
	assert_true (b[0] == 42);
	assert_true (b[1] == 43);

	ent_session_free (s);
	ent_session_free (NULL);
	ent_model_free (model);
}
