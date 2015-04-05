#include "test/ent_test.h"
#include "table.h"
#include "array.h"
#include "alloc.h"

#include <errno.h>
#include <stdint.h>

static void
null_table_sets_einval()
{
	errno = 0;
	assert_true (ent_table_len (NULL) == 0);
	assert_true (errno == EINVAL);

	errno = 0;
	assert_true (ent_table_column (NULL, "test", 1) == NULL);
	assert_true (errno == EINVAL);

	errno = 0;
	ent_table_free (NULL);
	assert_true (errno == EINVAL);

	errno = 0;
	ent_table_delete (NULL, NULL);
	assert_true (errno == EINVAL);

	errno = 0;
	assert_true (ent_table_column_info (NULL, 0, NULL) == NULL);
	assert_true (errno == EINVAL);

	errno = 0;
	ent_table_incref (NULL);
	assert_true (errno == EINVAL);

	errno = 0;
	assert_true (ent_table_columns_len (NULL) == 0);
	assert_true (errno == EINVAL);

	errno = 0;
	assert_true (ent_table_grow (NULL, 1) == -1);
	assert_true (errno = EINVAL);
}

static void
new_table_is_empty()
{
	struct ent_table * table = ent_table_alloc();

	errno = 0;
	assert_true (ent_table_len (table) == 0);
	assert_true (errno == 0);

	errno = 0;
	assert_true (ent_table_columns_len (table) == 0);
	assert_true (errno == 0);

	ent_table_free (table);
}

static void
column_info_can_be_retrieved()
{
	struct ent_table * table = ent_table_alloc();

	assert_true (ent_table_column (table, "int8", sizeof (int8_t)) != NULL);
	assert_true (ent_table_column (table, "int16", sizeof (int16_t)) != NULL);
	assert_true (ent_table_column (table, "int32", sizeof (int32_t)) != NULL);
	assert_true (ent_table_column (table, "int64", sizeof (int64_t)) != NULL);

	assert_true (ent_table_columns_len (table) == 4);

	size_t width;
	assert_true (strcmp ("int8", ent_table_column_info (table, 0, &width)) == 0);
	assert_true (width == 1);
	assert_true (strcmp ("int16", ent_table_column_info (table, 1, &width)) == 0);
	assert_true (width == 2);
	assert_true (strcmp ("int32", ent_table_column_info (table, 2, &width)) == 0);
	assert_true (width == 4);
	assert_true (strcmp ("int64", ent_table_column_info (table, 3, &width)) == 0);
	assert_true (width == 8);

	ent_table_free (table);
}

static void
invalid_column_id_sets_einval()
{
	struct ent_table * table = ent_table_alloc();
	size_t width = 1;

	errno = 0;
	assert_true (ent_table_column_info (table, 1, &width) == NULL);
	assert_true (width == 0);
	assert_true (errno == EINVAL);

	ent_table_free (table);
}

static int
deletion_handles_out_of_memory()
{
	struct ent_table * table = ent_table_alloc();

	if (table == NULL)
	{
		return -1;
	}

	struct ent_array * numbers = ent_table_column (table, "number",  sizeof (int));

	if (numbers == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	if (ent_table_grow (table, 64) == -1)
	{
		ent_table_free (table);
		return -1;
	}

	assert_true (ent_table_len (table) == 64);
	assert_true (ent_array_len (numbers) == 64);

	struct ent_rlist * delete = ent_rlist_alloc();

	if (delete == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	if (ent_rlist_append (delete, 0, 1) == -1)
	{
		ent_rlist_free (delete);
		ent_table_free (table);
		return -1;
	}

	if (ent_table_delete (table, delete) == -1)
	{
		ent_rlist_free (delete);
		ent_table_free (table);
		return -1;
	}

	ent_rlist_free (delete);

	assert_true (ent_table_len (table) == 63);
	numbers = ent_table_column (table, "number", sizeof (int));
	assert_true (numbers != NULL);
	assert_true (ent_array_len (numbers) == 63);

	ent_table_free (table);
	return 0;
}

static int
table_general_test()
{
	// Create a table
	struct ent_table * table = ent_table_alloc();

	if (table == NULL)
	{
		return -1;
	}

	assert_true (table != NULL);
	assert_true (ent_table_len (table) == 0);

	// Add two columns
	struct ent_array * names = ent_table_column (table, "name",  sizeof (char*));

	if (names == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	assert_true (names != NULL);

	if (ent_table_grow (table, 4) == -1)
	{
		ent_table_free (table);
		return -1;
	}

	assert_true (ent_table_len (table) == 4);
	assert_true (ent_array_len (names) == 4);

	struct ent_array * score = ent_table_column (table, "hits", sizeof (uint8_t));

	if (score == NULL)
	{
		ent_table_free (table);
		return -1;
	}

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

	if (delete == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	assert_true (delete != NULL);

	if (ent_rlist_append (delete, 0, 1) == -1)
	{
		ent_rlist_free (delete);
		ent_table_free (table);
		return -1;
	}

	if (ent_rlist_append (delete, 2, 3) == -1)
	{
		ent_rlist_free (delete);
		ent_table_free (table);
		return -1;
	}

	if (ent_table_delete (table, delete) == -1)
	{
		ent_rlist_free (delete);
		ent_table_free (table);
		return -1;
	}

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
	return 0;
}

void table_test()
{
	null_table_sets_einval();
	new_table_is_empty();
	column_info_can_be_retrieved();
	invalid_column_id_sets_einval();

	int (* functions[])() =
	{
		table_general_test,
		deletion_handles_out_of_memory,
	};

	for (size_t f = 0; f < sizeof (functions) / sizeof (*functions); ++f)
	{
		size_t zero = ent_alloc_count();
		assert_true (functions[f]() == 0);
		size_t used = ent_alloc_count() - zero;

		for (size_t m = 1; m <= used; ++m)
		{
			ent_alloc_artificial_fail (ent_alloc_count() + m);
			errno = 0;
			assert_true (functions[f]() == -1);
			assert_true (errno == ENOMEM);
		}
	}
}
