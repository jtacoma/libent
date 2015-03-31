#include "test/ent_test.h"
#include "column.h"

void
column_test()
{
	struct ent_column * names = ent_column_alloc (sizeof (char*));
	assert_true (names != NULL);

	assert_true (ent_column_set_len (names, 4) == 0);
	assert_true (ent_column_len (names) == 4);

	void const ** dst = ent_column_ref (names);
	assert_true (dst != NULL);

	char const * src[] = { "Lana", "Archer", "Cyril" };
	for (size_t i = 0; i < sizeof (src) / sizeof (*src); ++i, ++dst)
	{
		*dst = src[i];
	}

	*dst = "Randy";

	struct ent_column * filtered = ent_column_alloc (sizeof (char*));
	assert_true (ent_column_set_len (filtered, 3) == 0);

	struct ent_rlist * keep = ent_rlist_alloc();
	assert_true (keep != NULL);
	assert_true (ent_rlist_select (keep, ent_column_ref (filtered), ent_column_get (names), sizeof (char*)) == 0);
	assert_true (ent_rlist_select (keep, ent_column_ref (filtered), ent_column_get (names), 0) == -1);
	assert_true (ent_rlist_select (keep, ent_column_ref (filtered), NULL, sizeof (char*)) == -1);
	assert_true (ent_rlist_select (keep, NULL, ent_column_get (names), sizeof (char*)) == -1);
	assert_true (ent_rlist_select (NULL, ent_column_ref (filtered), ent_column_get (names), sizeof (char*)) == -1);
	ent_rlist_free (keep);

	keep = ent_rlist_alloc();
	assert_true (keep != NULL);
	assert_true (ent_rlist_append (keep, 0, 1) == 0);
	assert_true (ent_rlist_append (keep, 2, 4) == 0);
	assert_true (ent_rlist_select (keep, ent_column_ref (filtered), ent_column_get (names), sizeof (char*)) == 0);
	ent_rlist_free (keep);

	size_t actual_len = ent_column_len (filtered);
	assert_true (actual_len == 3);

	void ** actual = ent_column_ref (filtered);
	assert_true (actual != NULL);

	char const * expected[] = { "Lana", "Cyril", "Randy" };
	for (size_t i = 0; i < actual_len; ++i)
	{
		char const * s = actual[i];
		assert_true (s != NULL);
		assert_true (strcmp (s, expected[i]) == 0);
	}

	ent_column_free (filtered);
	ent_column_free (names);
	ent_column_free (NULL);
}
