#include "test/ent_test.h"
#include "table.h"
#include "array.h"

void
table_test()
{
	// Create a table
	struct ent_table * table = ent_table_alloc (4);
	assert_true (table != NULL);
	assert_true (ent_table_len (table) == 4);
	assert_true (ent_table_len (NULL) == 0);

	// Add two columns
	struct ent_array * names = ent_table_column (table, "name",  sizeof (char*));
	assert_true (names != NULL);
	assert_true (ent_array_len (names) == 4);
	struct ent_array * score = ent_table_column (table, "hits", sizeof (uint8_t));
	assert_true (table != NULL);
	assert_true (ent_array_len (score) == 4);

	// NULL/zero arguments will not work
	assert_true (ent_table_column (table, "name", 0) == NULL);
	assert_true (ent_table_column (table, NULL, sizeof (char*)) == NULL);
	assert_true (ent_table_column (NULL, "name", sizeof (char*)) == NULL);

	// A duplicate column of different size
	assert_true (ent_table_column (table, "name", sizeof (char*) * 2) == NULL);

	// A duplicate column of same size
	assert_true (ent_table_column (table, "name", sizeof (char*)) != NULL);

	// Get pointers to the initially zero-filled column data
	void const ** names_dst = ent_array_ref (names);
	assert_true (names_dst != NULL);
	uint8_t * scores_dst = ent_array_ref (score);
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
	assert_true (ent_table_delete (table, NULL) == -1);
	assert_true (ent_table_delete (NULL, delete) == -1);
	ent_rlist_free (delete);

	// Verify that the entities have been deleted
	assert_true (ent_table_len (table) == 2);
	names = ent_table_column (table, "name", sizeof (char*));
	assert_true (names != NULL);
	assert_true (ent_array_len (names) == 2);
	score = ent_table_column (table, "hits", sizeof (uint8_t));
	assert_true (table != NULL);
	assert_true (ent_array_len (score) == 2);

	names = ent_table_column (table, "name", sizeof (char*));
	names_dst = ent_array_ref (names);
	assert_true (names_dst != NULL);
	score = ent_table_column (table, "hits", sizeof (uint8_t));
	scores_dst = ent_array_ref (score);
	assert_true (scores_dst != NULL);

	ent_table_free (table);
	ent_table_free (NULL);
}
