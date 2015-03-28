#include "test/ent_test.h"
#include "ent_internal.h"

void
column_test()
{
	struct ent_column * names = ent_column_alloc ("bytes", 4);
	assert_true (names != NULL);
	assert_true (ent_column_len (names) == 4);

	void const ** dst = ent_column_ref (names);
	assert_true (dst != NULL);

	char const * src[] = { "Lana", "Archer", "Cyril" };
	for (size_t i = 0; i < sizeof (src) / sizeof (*src); ++i, ++dst)
	{
		*dst = src[i];
	}

	*dst = "Randy";

	struct ent_column * filtered = ent_column_alloc ("bytes", 3);

	struct ent_rlist * want = ent_rlist_alloc();
	assert_true (want != NULL);
	assert_true (ent_column_select (filtered, names, want) == -1);
	assert_true (ent_rlist_append (want, 0, 6) == 0);
	assert_true (ent_column_select (filtered, names, want) == -1);
	ent_rlist_free (want);
	want = ent_rlist_alloc();
	assert_true (ent_rlist_append (want, 0, 1) == 0);
	assert_true (ent_rlist_append (want, 2, 4) == 0);
	assert_true (ent_column_select (filtered, names, want) == 0);

	size_t actual_len = ent_column_len (filtered);
	assert_true (actual_len == 3);

	void ** actual = ent_column_ref (filtered);
	assert_true (actual != NULL);

	char const * expected[] = { "Lana", "Cyril", "Randy" };
	for (size_t i = 0; i < actual_len; ++i)
	{
		char const * s = actual[i];
		printf ("i=%lu\n", i);
		assert_true (s != NULL);
		printf ("s[i]=%s\n", s);
		assert_true (strcmp (s, expected[i]) == 0);
	}

	ent_column_free (filtered);
	ent_rlist_free (want);
	ent_column_free (names);
}
