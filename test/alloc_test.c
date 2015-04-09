#include "test/ent_test.h"

void
alloc_counts_allocations (void)
{
	// Note: this test is not thead-safe because the number of allocations
	// might be affected by other tests running in parallel.
	size_t zero = ent_alloc_count();
	void * mem = NULL;
	for (size_t i = 0; i < 20; ++i)
	{
		assert (ent_alloc (&mem, 4) == 0);
		assert (mem);
		ent_alloc (&mem, 0);
		assert (ent_alloc_count() - zero == i + 1);
	}
}

void
alloc_fails_when_told_to_fail (void)
{
	// Note: this test is not thead-safe because the number of allocations
	// might be affected by other tests running in parallel.
	size_t zero = ent_alloc_count();
	ent_alloc_artificial_fail (zero + 2);

	void * mem = NULL;
	assert (ent_alloc (&mem, 4) == 0);
	assert (mem);
	ent_alloc (&mem, 0);

	void * fail = NULL;
	assert (ent_alloc (&fail, 4) == -1);
	assert (fail == NULL);
	assert (ent_alloc_count() - zero == 2);
}

void
null_ptr_set_einval (void)
{
	errno = 0;
	ent_alloc (NULL, 16);
	assert (errno == EINVAL);

	errno = 0;
	ent_alloc (NULL, 0);
	assert (errno == EINVAL);
}
