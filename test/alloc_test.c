#include "test/ent_test.h"
#include "alloc.h"
#include <errno.h>

static void
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

static void
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

static void
null_ptr_set_einval()
{
	errno = 0;
	ent_alloc (NULL, 16);
	assert_true (errno == EINVAL);

	errno = 0;
	ent_alloc (NULL, 0);
	assert_true (errno == EINVAL);
}

void
alloc_test()
{
	alloc_counts_allocations();
	alloc_fails_when_told_to_fail();
	null_ptr_set_einval();
}
