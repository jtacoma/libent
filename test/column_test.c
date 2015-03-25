#include "test/ent_test.h"

void
column_test ()
{
	struct ent_column * names = ent_column_alloc ("bytes", 4);
	assert_true (names != NULL);

	size_t names_len = 0;
	struct ent_bytes * dst = ent_column_ref (names, &names_len);
	assert_true (dst != NULL);

	char const * src[] = { "Lana", "Archer", "Cyril" };
	for (size_t i = 0; i < sizeof(src) / sizeof(*src); ++i, ++dst)
	{
		ent_bytes_reset (dst, src[i], strlen(src[i]) + 1);

		size_t len;
		char const * result = ent_bytes_get (dst, &len);
		assert_true (len == strlen (src[i]) + 1);
		assert_true (strcmp (result, src[i]) == 0);
	}

	ent_bytes_reset (dst, "Randy", 6);
	size_t randy_len;
	assert_true (strcmp ("Randy", ent_bytes_get (dst, &randy_len)) == 0);
	assert_true (randy_len == 6);

	struct ent_column * filtered = ent_column_alloc("bytes", 3);

	struct ent_rlist * want = ent_rlist_alloc ();
	assert_true (want != NULL);
	assert_true (ent_column_select (filtered, names, want) == -1);
	assert_true (ent_rlist_append (want, 0, 6) == 0);
	assert_true (ent_column_select (filtered, names, want) == -1);
	ent_rlist_free(want);
	want = ent_rlist_alloc ();
	assert_true (ent_rlist_append (want, 0, 1) == 0);
	assert_true (ent_rlist_append (want, 2, 4) == 0);
	assert_true (ent_column_select (filtered, names, want) == 0);

	size_t actual_len;
	struct ent_bytes * actual = ent_column_ref (filtered, &actual_len);
	assert_true (actual_len == 3);
	assert_true (actual != NULL);

	char const * expected[] = { "Lana", "Cyril", "Randy" };
	for (size_t i = 0; i < actual_len; ++i)
	{
		size_t len;
		char const * s = ent_bytes_get(&actual[i], &len);
		printf("i=%lu\n", i);
		assert_true (s != NULL);
		printf("s[i]=%s\n", s);
		assert_true (len == strlen(s) + 1);
		assert_true (strcmp (s, expected[i]) == 0);
	}

	ent_column_free (filtered);
	ent_rlist_free (want);
	ent_column_free (names);
}
