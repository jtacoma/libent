#include "test/ent_test.h"
#include "table.h"

void session_test()
{
	struct ent_model * model = ent_model_alloc();
	assert_true (model != NULL);

	struct ent_session * s = ent_session_alloc (model);
	assert_true (s != NULL);

	struct ent_table * items = ent_session_table (s, "items");
	assert_true (items != NULL);

	struct ent_column const * column_w = ent_session_column_r (s, items, "a", "int32");
	assert_true (column_w == NULL);

	struct ent_column * column_b = ent_session_column_w (s, items, "b", "float64");
	assert_true (column_b != NULL);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_session_table_len (s, items) == 0);
	goto end;
	assert_true (ent_session_table_grow (s, items, 2) == 0);

	//int32_t const * a = ent_column_get (column_w);
	//assert_true (a);
	//assert_true (a[0] == 0);
	//assert_true (a[1] == 0);

	double * b = ent_column_ref (column_b);
	assert_true (b);
	b[0] = 42;
	b[1] = 43;
	ent_session_free (s);

	s = ent_session_alloc (model);
	assert_true (s != NULL);
	items = ent_session_table (s, "items");
	assert_true (items != NULL);
	column_b = ent_session_column_w (s, items, "b", "float64");
	assert_true (column_b != NULL);

	assert_true (ent_session_lock (s) == 0);
	assert_true (ent_table_len (items) == 2);

	b = ent_column_ref (column_b);
	assert_true (b);
	assert_true (b[0] == 42);
	assert_true (b[1] == 43);

end:
	ent_session_free (s);
	ent_model_free (model);
}
