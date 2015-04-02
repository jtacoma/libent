#include "test/ent_test.h"
#include "table.h"

struct my_info
{
	int items;
	int column_b;
};

static int
my_loader (
    struct ent_session * s,
    void * void_info)
{
	struct my_info * info = void_info;
	int items = info->items;
	int column_b = info->column_b;

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

	return 0;
}

static int
my_checker (
    struct ent_session * s,
    void * void_info)
{
	struct my_info * info = void_info;
	int items = info->items;
	int column_b = info->column_b;

	assert_true (ent_session_table_len (s, items) == 2);

	double * b = ent_session_column_write (s, items, column_b);
	assert_true (b);
	assert_true (b[0] == 42);
	assert_true (b[1] == 43);

	return 0;
}

void session_test()
{
	assert_true (ent_processor_alloc (NULL) == NULL);

	struct ent_model * model = ent_model_alloc();
	assert_true (model != NULL);

	struct ent_processor * p = ent_processor_alloc (model);
	assert_true (p != NULL);

	struct my_info info;

	info.items = ent_processor_use_table (p, "items", "w");
	assert_true (info.items >= 0);
	assert_true (ent_processor_use_table (p, NULL, "w") == -1);
	assert_true (ent_processor_use_table (NULL, "items", "w") == -1);

	assert_true (ent_processor_use_column (p, info.items, "a", 0, "r") == -1);
	assert_true (ent_processor_use_column (p, info.items, NULL, 1, "r") == -1);
	assert_true (ent_processor_use_column (p, -1, "a", 1, "r") == -1);
	assert_true (ent_processor_use_column (NULL, info.items, "a", 1, "r") == -1);

	assert_true (ent_processor_use_column (p, info.items, "b", 0, "w") == -1);
	assert_true (ent_processor_use_column (p, info.items, NULL, 8, "w") == -1);
	assert_true (ent_processor_use_column (p, -1, "b", 8, "w") == -1);
	assert_true (ent_processor_use_column (NULL, info.items, "b", 8, "w") == -1);
	info.column_b = ent_processor_use_column (p, info.items, "b", 8, "w");
	assert_true (info.column_b >= 0);

	ent_processor_set_function (p, my_loader, &info);

	ent_model_set_processor ("load", p);

	ent_processor_free (p);

	p = ent_processor_alloc (model);
	assert_true (p != NULL);
	info.items = ent_processor_use_table (p, "items", "w");
	assert_true (info.items >= 0);
	assert_true (ent_processor_use_column (p, info.items, NULL, 8, "w") == -1);
	assert_true (ent_processor_use_column (p, -1, "b", 8, "w") == -1);
	assert_true (ent_processor_use_column (NULL, info.items, "b", 8, "w") == -1);
	info.column_b = ent_processor_use_column (p, info.items, "b", 8, "w");
	assert_true (info.column_b != -1);

	ent_processor_set_function (p, my_checker, &info);

	ent_model_set_processor ("check", p);

	ent_processor_free (p);
	ent_processor_free (NULL);

	ent_model_free (model);
}
