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
}
