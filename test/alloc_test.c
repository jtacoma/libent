#include "test/ent_test.h"
#include "alloc.h"

void
alloc_counts_allocations()
{
	// Note: this test is not thead-safe because the number of allocations
	// might be affected by other tests running in parallel.
	size_t zero = ent_alloc_count();
	void * mem = NULL;
	for (size_t i = 0; i < 20; ++i)
	{
		assert_true (ent_alloc (&mem, 4) == 0);
		assert_true (mem);
		ent_alloc (&mem, 0);
		assert_true (ent_alloc_count() - zero == i + 1);
	}
}

void
alloc_fails_when_told_to_fail()
{
	// Note: this test is not thead-safe because the number of allocations
	// might be affected by other tests running in parallel.
	size_t zero = ent_alloc_count();
	ent_alloc_artificial_fail (zero + 2);

	void * mem = NULL;
	assert_true (ent_alloc (&mem, 4) == 0);
	assert_true (mem);
	ent_alloc (&mem, 0);

	void * fail = NULL;
	assert_true (ent_alloc (&fail, 4) == -1);
	assert_true (fail == NULL);
	assert_true (ent_alloc_count() - zero == 2);
}

void
alloc_test()
{
	alloc_counts_allocations();
	alloc_fails_when_told_to_fail();
}
