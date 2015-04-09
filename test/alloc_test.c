#include "test/ent_test.h"

int
alloc_counts_allocations (void)
{
	size_t zero = ent_alloc_count();
	void * mem = NULL;

	for (size_t i = 0; i < 20; ++i)
	{
		if (ent_alloc (&mem, 4) == -1)
		{
			return -1;
		}

		assert (mem);
		assert (ent_alloc_count() - zero == i + 1);
		ent_alloc (&mem, 0);
		assert (ent_alloc_count() - zero == i + 1);
	}

	return 0;
}

int
alloc_fails_when_told_to_fail (void)
{
	size_t zero = ent_alloc_count();
	ent_alloc_set_fail_at (zero + 2);

	void * mem = NULL;
	assert (ent_alloc (&mem, 4) == 0);
	assert (mem);
	ent_alloc (&mem, 0);

	void * fail = NULL;
	assert (ent_alloc (&fail, 4) == -1);
	assert (fail == NULL);
	assert (ent_alloc_count() - zero == 2);

	return 0;
}

int
alloc_with_null_pptr_sets_einval (void)
{
	errno = 0;
	ent_alloc (NULL, 16);
	assert (errno == EINVAL);

	errno = 0;
	ent_alloc (NULL, 0);
	assert (errno == EINVAL);

	return 0;
}
