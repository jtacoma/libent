#include "test/ent_test.h"
#include "alloc.h"
#include "array.h"
#include "errno.h"

typedef size_t index;
ent_array_typed (index);

typedef char const * string;
ent_array_typed (string);

static void
new_array_has_specified_width()
{
	errno = 0;

	struct ent_array * array = ent_array_alloc (32);

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (ent_array_width (array) == 32);

	ent_array_free (array);
}

static void
new_array_is_empty()
{
	errno = 0;

	struct ent_array * array = ent_array_alloc (1);

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (errno == 0);
	assert (array);

	assert (ent_array_len (array) == 0);
	assert (ent_array_get (array) == NULL);
	assert (ent_array_ref (array) == NULL);
	ent_array_free (array);
}

static void
resized_array_is_set_to_zero()
{
	errno = 0;

	struct ent_index_array * array = ent_index_array_alloc();

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (errno == 0);
	assert (array);

	errno = 0;

	if (ent_index_array_set_len (array, 4) == -1)
	{
		if (errno == ENOMEM)
		{
			ent_index_array_free (array);
			return;
		}

		assert (errno == 0);
	}

	assert (ent_index_array_len (array) == 4);

	for (size_t i = 0; i < 4; ++i)
	{
		assert (ent_index_array_get (array)[i] == 0);
	}

	errno = 0;

	if (ent_index_array_set_len (array, 16) == -1)
	{
		if (errno == ENOMEM)
		{
			ent_index_array_free (array);
			return;
		}

		assert (errno == 0);
	}

	assert (ent_index_array_len (array) == 16);

	for (size_t i = 0; i < 16; ++i)
	{
		assert (ent_index_array_get (array)[i] == 0);
	}

	ent_index_array_free (array);
}

static void
resized_array_retains_data()
{
	errno = 0;

	struct ent_index_array * array = ent_index_array_alloc();

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (errno == 0);
	assert (array);

	if (ent_index_array_set_len (array, 8) == -1)
	{
		if (errno == ENOMEM)
		{
			ent_index_array_free (array);
			return;
		}

		assert (errno == 0);
	}

	for (size_t i = 0; i < 8; ++i)
	{
		ent_index_array_ref (array)[i] = i + 1;
		assert (ent_index_array_get (array)[i] == i + 1);
	}

	if (ent_index_array_set_len (array, 16) == -1)
	{
		if (errno == ENOMEM)
		{
			ent_index_array_free (array);
			return;
		}

		assert (errno == 0);
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
}

static void
copied_array_keeps_original_data()
{
	errno = 0;

	struct ent_array * array = ent_array_alloc (sizeof (int));

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (errno == 0);
	assert (array);

	if (ent_array_set_len (array, 8) == -1)
	{
		if (errno == ENOMEM)
		{
			ent_array_free (array);
			return;
		}

		assert (errno == 0);
	}

	struct ent_array * copy = ent_array_cpy_alloc (array);

	if (!copy && errno == ENOMEM)
	{
		ent_array_free (array);
		return;
	}

	assert (errno == 0);
	assert (copy);

	assert (ent_array_get (copy) != ent_array_get (array));
	assert (ent_array_len (copy) == 8);

	// TODO: verify contents of memory

	ent_array_free (copy);
	ent_array_free (array);
}

static void
invalid_arguments_set_EINVAL()
{
	errno = 0;
	assert (ent_array_cpy_alloc (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_alloc (0) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_get (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_len (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_ref (NULL) == NULL);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_set_len (NULL, 1) == -1);
	assert (errno == EINVAL);

	errno = 0;
	assert (ent_array_width (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	ent_array_free (NULL);
	assert (errno == EINVAL);
}

static void
truncated_array_returns_null()
{
	errno = 0;

	struct ent_array * array = ent_array_alloc (1);

	if (!array && errno == ENOMEM)
	{
		return;
	}

	assert (errno == 0);
	assert (array);

	if (ent_array_set_len (array, 4) == -1 && errno == ENOMEM)
	{
		ent_array_free (array);
		return;
	}

	assert (ent_array_len (array) == 4);
	assert (ent_array_set_len (array, 0) == 0);
	assert (ent_array_len (array) == 0);

	errno = 0;
	assert (ent_array_get (array) == NULL);
	assert (errno == 0);

	errno = 0;
	assert (ent_array_ref (array) == NULL);
	assert (errno == 0);

	ent_array_free (array);
}

typedef void (*test_function)();

test_function functions [] =
{
	new_array_has_specified_width,
	new_array_is_empty,
	resized_array_is_set_to_zero,
	resized_array_retains_data,
	copied_array_keeps_original_data,
	invalid_arguments_set_EINVAL,
	truncated_array_returns_null,
};

void
array_test()
{
	size_t functions_len = sizeof (functions) / sizeof (*functions);

	for (size_t i = 0; i < functions_len; ++i)
	{
		size_t zero = ent_alloc_count();
		functions[i]();
		size_t used = ent_alloc_count() - zero;

		for (size_t allow = 0; allow < used; ++allow)
		{
			size_t zero = ent_alloc_count();
			ent_alloc_artificial_fail (zero + 1 + allow);
			errno = 0;
			functions[i]();
			assert (errno == ENOMEM);
		}

		ent_alloc_artificial_fail (0);
	}
}
