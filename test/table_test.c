#include "test/ent_test.h"
#include "table.h"
#include "column.h"

void
table_test()
{
	// Create a table
	struct ent_table * table = ent_table_alloc (4);
	assert_true (table != NULL);
	assert_true (ent_table_len (table) == 4);

	// Add two columns
	struct ent_column * names = ent_table_add_column (table, "name", "bytes");
	assert_true (names != NULL);
	assert_true (ent_column_len (names) == 4);
	struct ent_column * score = ent_table_add_column (table, "hits", "uint8");
	assert_true (table != NULL);
	assert_true (ent_column_len (score) == 4);

	// Add a duplicate column
	struct ent_column * bad = ent_table_add_column (table, "name", "bytes");
	assert_true (bad == NULL);

	// Get pointers to the initially zero-filled column data
	void const ** names_dst = ent_column_ref (names);
	assert_true (names_dst != NULL);
	uint8_t * scores_dst = ent_column_ref (score);
	assert_true (scores_dst != NULL);

	// Replace the zero values with some sample data
	char const * names_src[] = { "Lana", "Archer", "Cyril", "Carol" };
	for (size_t i = 0; i < 4; ++i)
	{
		assert_true (names_dst[i] == NULL);
		assert_true (scores_dst[i] == 0);
		names_dst[i] = names_src[i];
		scores_dst[i] = 10 + i;
	}

	// Delete some entities
	struct ent_rlist * delete = ent_rlist_alloc();
	assert_true (delete != NULL);
	assert_true (ent_rlist_append (delete, 0, 1) == 0);
	assert_true (ent_rlist_append (delete, 3, 4) == 0);
	assert_true (ent_table_delete (table, delete) == 0);
	ent_rlist_free (delete);

	// Verify that the entities have been deleted
	printf ("len=%lu\n", ent_table_len (table));
	assert_true (ent_table_len (table) == 2);
	assert_true (ent_column_len (ent_table_column (table, "name", "bytes")) == 2);
	assert_true (ent_column_len (ent_table_column (table, "hits", "uint8")) == 2);

	names = ent_table_column (table, "name", "bytes");
	names_dst = ent_column_ref (names);
	assert_true (names_dst != NULL);
	score = ent_table_column (table, "hits", "uint8");
	scores_dst = ent_column_ref (score);
	assert_true (scores_dst != NULL);

	ent_table_free (table);
}
