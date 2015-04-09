#include "test/ent_test.h"

ent_array_typed (size_t, index);

int
new_array_has_specified_width (void)
{
	struct ent_array * array = ent_array_alloc (32);

	if (!array)
	{
		return -1;
	}

	assert (ent_array_width (array) == 32);

	ent_array_free (array);
	return 0;
}

int
new_array_is_empty (void)
{
	struct ent_array * array = ent_array_alloc (1);

	if (!array)
	{
		return -1;
	}

	assert (ent_array_len (array) == 0);
	assert (ent_array_get_const (array) == NULL);
	assert (ent_array_get (array) == NULL);

	ent_array_free (array);
	return 0;
}

int
resized_array_is_set_to_zero (void)
{
	struct ent_index_array * array = ent_index_array_alloc();

	if (!array)
	{
		return -1;
	}

	if (ent_index_array_set_len (array, 4) == -1)
	{
		ent_index_array_free (array);
		return -1;
	}

	assert (ent_index_array_len (array) == 4);

	for (size_t i = 0; i < 4; ++i)
	{
		assert (ent_index_array_get (array)[i] == 0);
	}

	if (ent_index_array_set_len (array, 16) == -1)
	{
		ent_index_array_free (array);
		return -1;
	}

	assert (ent_index_array_len (array) == 16);

	for (size_t i = 0; i < 16; ++i)
	{
		assert (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_free (array);
	return 0;
}

int
resized_array_retains_data (void)
{
	struct ent_index_array * array = ent_index_array_alloc();

	if (!array)
	{
		return -1;
	}

	if (ent_index_array_set_len (array, 8) == -1)
	{
		ent_index_array_free (array);
		return -1;
	}

	for (size_t i = 0; i < 8; ++i)
	{
		ent_index_array_get (array)[i] = i + 1;
		assert (ent_index_array_get (array)[i] == i + 1);
	}

	if (ent_index_array_set_len (array, 16) == -1)
	{
		ent_index_array_free (array);
		return -1;
	}

	for (size_t i = 0; i < 8; ++i)
	{
		assert (ent_index_array_get (array)[i] == i + 1);
	}

	for (size_t i = 8; i < 16; ++i)
	{
		assert (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_free (array);
	return 0;
}

int
copied_array_keeps_original_data (void)
{
	struct ent_array * array = ent_array_alloc (sizeof (int));

	if (!array)
	{
		return -1;
	}

	if (ent_array_set_len (array, 8) == -1)
	{
		ent_array_free (array);
		return -1;
	}

	struct ent_array * copy = ent_array_cpy_alloc (array);

	if (!copy)
	{
		ent_array_free (array);
		return -1;
	}

	assert (ent_array_get_const (copy) != ent_array_get_const (array));
	assert (ent_array_len (copy) == 8);

	// TODO: verify contents of memory

	ent_array_free (copy);
	ent_array_free (array);
	return 0;
}

int
invalid_arguments_set_einval (void)
{
	errno = 0;
	assert (ent_array_cpy_alloc (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_alloc (0) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_get_const (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_len (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_get (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_set_len (NULL, 1) == -1);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_preallocate (NULL, 1) == -1);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_shrink (NULL) == -1);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_width (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	ent_array_free (NULL);
	assert (errno == EINVAL);

	struct ent_array * array = ent_array_alloc (1);

	if (!array)
	{
		return -1;
	}

	struct ent_rlist * keep = ent_rlist_alloc();

	if (!keep)
	{
		ent_array_free (array);
		return -1;
	}

	if (ent_rlist_append (keep, 0, 1) == -1)
	{
		ent_rlist_free (keep);
		ent_array_free (array);
		return -1;
	}

	errno = 0;
	assert (ent_array_select_in_place (array, keep) == -1);
	assert (errno == EINVAL);

	ent_rlist_free (keep);
	ent_array_free (array);
	return 0;
}

int
truncated_and_shrunk_array_returns_null (void)
{
	struct ent_array * array = ent_array_alloc (1);

	if (!array)
	{
		return -1;
	}

	if (ent_array_set_len (array, 4) == -1)
	{
		ent_array_free (array);
		return -1;
	}

	assert (ent_array_len (array) == 4);
	assert (ent_array_set_len (array, 0) == 0);
	assert (ent_array_len (array) == 0);

	errno = 0;
	assert (ent_array_get_const (array) != NULL);
	assert (errno == 0);

	errno = 0;
	assert (ent_array_get (array) != NULL);
	assert (errno == 0);

	// Shrinking an empty array is a no-op.
	assert (ent_array_shrink (array) == 0);

	errno = 0;
	assert (ent_array_get_const (array) == NULL);
	assert (errno == 0);

	errno = 0;
	assert (ent_array_get (array) == NULL);
	assert (errno == 0);

	ent_array_free (array);
	return 0;
}

int
array_can_shrink_safely (void)
{
	struct ent_array * array = ent_array_alloc (1);

	if (!array)
	{
		return -1;
	}

	if (ent_array_set_len (array, 4) == -1)
	{
		ent_array_free (array);
		return -1;
	}

	assert (ent_array_len (array) == 4);
	assert (ent_array_set_len (array, 1) == 0);

	if (ent_array_shrink (array) == -1)
	{
		ent_array_free (array);
		return -1;
	}

	// If an array's length has not been changed since the last time it
	// was successfully shrunk then shrinking should always succeed
	// because nothing needs to be done.
	assert (ent_array_shrink (array) == 0);

	ent_array_free (array);
	return 0;
}
