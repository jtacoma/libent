#include "test/ent_test.h"

void
table_test()
{
	// Create a table
	struct ent_table * table = ent_table_alloc (4);
	assert_true (table != NULL);
	assert_true (ent_table_len (table) == 4);

	// Add a column
	struct ent_column * names = ent_table_add_column (table, "name", "bytes");
	assert_true (names != NULL);
	struct ent_column * score = ent_table_add_column (table, "hits", "uint8");
	assert_true (table != NULL);

	// Add a duplicate column
	struct ent_column * bad = ent_table_add_column (table, "name", "bytes");
	assert_true (bad == NULL);

	// Get pointers to the initially zero-filled column data
	size_t len = 0;
	void const ** names_dst = ent_column_ref (names, &len);
	assert_true (names_dst != NULL);
	assert_true (len == 4);
	uint8_t * scores_dst = ent_column_ref (score, &len);
	assert_true (scores_dst != NULL);
	assert_true (len == 4);

	// Replace the zero values with some sample data
	char const * names_src[] = { "Lana", "Archer", "Cyril", "Carol" };
	for (size_t i = 0; i < 4; ++i)
	{
		assert_true (names_dst[i] == NULL);
		assert_true (scores_dst[i] == 0);
		names_dst[i] = names_src[i];
		scores_dst[i] = 10 + i;
	}

	ent_table_free (table);
}
