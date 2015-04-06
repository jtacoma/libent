#include "test/ent_test.h"

static void
null_table_sets_einval()
{
	errno = 0;
	assert (ent_table_len (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_table_column (NULL, "test", 1) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	ent_table_free (NULL);
	assert (errno == EINVAL);

	errno = 0;
	ent_table_delete (NULL, NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_table_column_info (NULL, 0, NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	ent_table_incref (NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_table_columns_len (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_table_pre_grow (NULL, 1) == -1);
	assert (errno = EINVAL);

	errno = 0;
	assert (ent_table_grow (NULL, 1) == -1);
	assert (errno = EINVAL);
}

static void
new_table_is_empty()
{
	struct ent_table * table = ent_table_alloc();

	errno = 0;
	assert (ent_table_len (table) == 0);
	assert (errno == 0);

	errno = 0;
	assert (ent_table_columns_len (table) == 0);
	assert (errno == 0);

	ent_table_free (table);
}

static void
adding_zero_length_is_ok()
{
	struct ent_table * table = ent_table_alloc();
	assert (table);
	assert (ent_table_grow (table, 0) == 0);
	ent_table_free (table);
}

static void
column_info_can_be_retrieved()
{
	struct ent_table * table = ent_table_alloc();

	assert (ent_table_column (table, "int8", sizeof (int8_t)) != NULL);
	assert (ent_table_column (table, "int16", sizeof (int16_t)) != NULL);
	assert (ent_table_column (table, "int32", sizeof (int32_t)) != NULL);
	assert (ent_table_column (table, "int64", sizeof (int64_t)) != NULL);

	assert (ent_table_columns_len (table) == 4);

	size_t width;
	assert (strcmp ("int8", ent_table_column_info (table, 0, &width)) == 0);
	assert (width == 1);
	assert (strcmp ("int16", ent_table_column_info (table, 1, &width)) == 0);
	assert (width == 2);
	assert (strcmp ("int32", ent_table_column_info (table, 2, &width)) == 0);
	assert (width == 4);
	assert (strcmp ("int64", ent_table_column_info (table, 3, &width)) == 0);
	assert (width == 8);

	ent_table_free (table);
}

static void
invalid_column_id_sets_einval()
{
	struct ent_table * table = ent_table_alloc();
	size_t width = 1;

	errno = 0;
	assert (ent_table_column_info (table, 1, &width) == NULL);
	assert (width == 0);
	assert (errno == EINVAL);

	ent_table_free (table);
}

static int
insertion_handles_out_of_memory()
{
	struct ent_table * dst = ent_table_alloc();

	if (!dst)
	{
		return -1;
	}

	struct ent_table * src = ent_table_alloc();

	if (!src)
	{
		ent_table_free (dst);
		return -1;
	}

	bool ok = false;

	do
	{
		// The destination table needs to have at least on extant column
		// to trigger one more line of code coverage.
		if (!ent_table_column (dst, "some-other-column", 1))
		{
			return -1;
		}

		if (ent_table_grow (src, 1) == -1)
		{
			break;
		}

		struct ent_array * number = ent_table_column (src, "number", sizeof (int));

		if (number == NULL)
		{
			break;
		}

		if (ent_table_insert (dst, src) == -1)
		{
			break;
		}

		assert (ent_table_len (dst) == 1);

		ok = true;
	}
	while (0);

	ent_table_free (src);
	ent_table_free (dst);
	return ok ? 0 : -1;
}

static int
deletion_handles_out_of_memory (size_t len, size_t start, size_t end)
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

	if (ent_table_grow (table, len) == -1)
	{
		ent_table_free (table);
		return -1;
	}

	assert (ent_table_len (table) == len);
	assert (ent_array_len (numbers) == len);

	struct ent_rlist * delete = ent_rlist_alloc();

	if (delete == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	if (ent_rlist_append (delete, start, end) == -1)
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

	assert (ent_table_len (table) == len - (end - start));
	numbers = ent_table_column (table, "number", sizeof (int));
	assert (numbers != NULL);
	assert (ent_array_len (numbers) == len - (end - start));

	ent_table_free (table);
	return 0;
}

static int
table_deletion_edge_cases()
{
	struct
	{
		size_t len;
		size_t start;
		size_t end;
	} rows [] =
	{
		{
			.len = 64,
			.start = 0,
			.end = 1,
		},
		{
			.len = 64,
			.start = 1,
			.end = 63,
		},
		{
			.len = 64,
			.start = 63,
			.end = 64,
		},
		{
			.len = 64,
			.start = 0,
			.end = 64,
		},
	};

	for (size_t i = 0; i < sizeof (rows) / sizeof (*rows); ++i)
	{
		if (deletion_handles_out_of_memory (rows[i].len, rows[i].start, rows[i].end) == -1)
		{
			return -1;
		}
	}

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

	assert (table != NULL);
	assert (ent_table_len (table) == 0);

	// Add two columns
	struct ent_array * names = ent_table_column (table, "name",  sizeof (char*));

	if (names == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	assert (names != NULL);

	if (ent_table_grow (table, 4) == -1)
	{
		ent_table_free (table);
		return -1;
	}

	assert (ent_table_len (table) == 4);
	assert (ent_array_len (names) == 4);

	struct ent_array * score = ent_table_column (table, "hits", sizeof (uint8_t));

	if (score == NULL)
	{
		ent_table_free (table);
		return -1;
	}

	assert (table != NULL);
	assert (ent_array_len (score) == 4);

	// NULL/zero arguments will not work
	assert (ent_table_column (table, "name", 0) == NULL);
	assert (ent_table_column (table, NULL, sizeof (char*)) == NULL);
	assert (ent_table_column (NULL, "name", sizeof (char*)) == NULL);

	// A duplicate column of different size
	assert (ent_table_column (table, "name", sizeof (char*) * 2) == NULL);

	// A duplicate column of same size
	assert (ent_table_column (table, "name", sizeof (char*)) != NULL);

	// Get pointers to the initially zero-filled column data
	void const ** names_dst = ent_array_get (names);
	assert (names_dst != NULL);
	uint8_t * scores_dst = ent_array_get (score);
	assert (scores_dst != NULL);

	// Replace the zero values with some sample data
	char const * names_src[] = { "Lana", "Archer", "Cyril", "Carol" };

	for (size_t i = 0; i < 4; ++i)
	{
		assert (names_dst[i] == NULL);
		assert (scores_dst[i] == 0);
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

	assert (delete != NULL);

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

	assert (ent_table_delete (table, NULL) == -1);
	assert (ent_table_delete (NULL, delete) == -1);
	ent_rlist_free (delete);

	// Verify that the entities have been deleted
	assert (ent_table_len (table) == 2);
	names = ent_table_column (table, "name", sizeof (char*));
	assert (names != NULL);
	assert (ent_array_len (names) == 2);
	score = ent_table_column (table, "hits", sizeof (uint8_t));
	assert (table != NULL);
	assert (ent_array_len (score) == 2);

	names = ent_table_column (table, "name", sizeof (char*));
	names_dst = ent_array_get (names);
	assert (names_dst != NULL);
	score = ent_table_column (table, "hits", sizeof (uint8_t));
	scores_dst = ent_array_get (score);
	assert (scores_dst != NULL);

	ent_table_free (table);
	ent_table_free (NULL);
	return 0;
}

void table_test()
{
	null_table_sets_einval();
	new_table_is_empty();
	adding_zero_length_is_ok();
	column_info_can_be_retrieved();
	invalid_column_id_sets_einval();

	int (* functions[])() =
	{
		insertion_handles_out_of_memory,
		table_deletion_edge_cases,
		table_general_test,
	};

	for (size_t f = 0; f < sizeof (functions) / sizeof (*functions); ++f)
	{
		size_t zero = ent_alloc_count();
		assert (functions[f]() == 0);
		size_t used = ent_alloc_count() - zero;

		for (size_t m = 1; m <= used; ++m)
		{
			ent_alloc_artificial_fail (ent_alloc_count() + m);
			errno = 0;
			assert (functions[f]() == -1);
			assert (errno == ENOMEM);
		}
	}
}
