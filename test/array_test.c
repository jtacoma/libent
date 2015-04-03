#include "test/ent_test.h"
#include "array.h"

typedef size_t index;
ent_array_typed (index);

typedef char const * string;
ent_array_typed (string);

static void
no_array_has_zero_width()
{
	struct ent_array * array = ent_array_alloc (0);
	assert_true (array == NULL);
	ent_array_free (array);
}

static void
new_array_has_specified_width()
{
	struct ent_array * array = ent_array_alloc (32);
	assert_true (ent_array_width (array) == 32);
	ent_array_free (array);
}

static void
new_array_is_empty()
{
	struct ent_array * array = ent_array_alloc (1);
	assert_true (ent_array_len (array) == 0);
	assert_true (ent_array_get (array) == NULL);
	assert_true (ent_array_ref (array) == NULL);
	ent_array_free (array);
}

static void
resized_array_is_set_to_zero()
{
	struct ent_index_array * array = ent_index_array_alloc();
	ent_index_array_set_len (array, 4);
	assert_true (ent_index_array_len (array) == 4);

	for (size_t i = 0; i < 4; ++i)
	{
		assert_true (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_set_len (array, 16);
	assert_true (ent_index_array_len (array) == 16);

	for (size_t i = 0; i < 16; ++i)
	{
		assert_true (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_free (array);
}

static void
resized_array_retains_data()
{
	struct ent_index_array * array = ent_index_array_alloc();
	ent_index_array_set_len (array, 8);

	for (size_t i = 0; i < 8; ++i)
	{
		ent_index_array_ref (array)[i] = i + 1;
		assert_true (ent_index_array_get (array)[i] == i + 1);
	}

	ent_index_array_set_len (array, 16);

	for (size_t i = 0; i < 8; ++i)
	{
		assert_true (ent_index_array_get (array)[i] == i + 1);
	}

	for (size_t i = 8; i < 16; ++i)
	{
		assert_true (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_free (array);
}

void
array_test()
{
	no_array_has_zero_width();
	new_array_has_specified_width();
	new_array_is_empty();
	resized_array_is_set_to_zero();
	resized_array_retains_data();

	struct ent_string_array * names = ent_string_array_alloc();
	assert_true (names != NULL);

	assert_true (ent_string_array_set_len (names, 4) == 0);
	assert_true (ent_string_array_len (names) == 4);

	char const ** dst = ent_string_array_ref (names);
	assert_true (dst != NULL);

	char const * src[] = { "Lana", "Archer", "Cyril" };
	for (size_t i = 0; i < sizeof (src) / sizeof (*src); ++i, ++dst)
	{
		*dst = src[i];
	}

	*dst = "Randy";

	struct ent_array * filtered = ent_array_alloc (ent_string_array_width());
	assert_true (ent_array_set_len (filtered, 3) == 0);

	struct ent_rlist * keep = ent_rlist_alloc();
	assert_true (keep != NULL);
	assert_true (ent_rlist_select (keep, ent_array_ref (filtered), ent_string_array_get (names), sizeof (char*)) == 0);
	assert_true (ent_rlist_select (keep, ent_array_ref (filtered), ent_string_array_get (names), 0) == -1);
	assert_true (ent_rlist_select (keep, ent_array_ref (filtered), NULL, sizeof (char*)) == -1);
	assert_true (ent_rlist_select (keep, NULL, ent_string_array_get (names), sizeof (char*)) == -1);
	assert_true (ent_rlist_select (NULL, ent_array_ref (filtered), ent_string_array_get (names), sizeof (char*)) == -1);
	ent_rlist_free (keep);

	keep = ent_rlist_alloc();
	assert_true (keep != NULL);
	assert_true (ent_rlist_append (keep, 0, 1) == 0);
	assert_true (ent_rlist_append (keep, 2, 4) == 0);
	assert_true (ent_rlist_select (keep, ent_array_ref (filtered), ent_string_array_get (names), sizeof (char*)) == 0);
	ent_rlist_free (keep);

	size_t actual_len = ent_array_len (filtered);
	assert_true (actual_len == 3);

	void ** actual = ent_array_ref (filtered);
	assert_true (actual != NULL);

	char const * expected[] = { "Lana", "Cyril", "Randy" };
	for (size_t i = 0; i < actual_len; ++i)
	{
		char const * s = actual[i];
		assert_true (s != NULL);
		assert_true (strcmp (s, expected[i]) == 0);
	}

	ent_array_free (filtered);
	ent_string_array_free (names);
	ent_array_free (NULL);
}
