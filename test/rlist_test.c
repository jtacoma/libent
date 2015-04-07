#include "test/ent_test.h"

static int
new_rlist_is_empty()
{
	struct ent_rlist * rlist = ent_rlist_alloc();

	assert (rlist != NULL);
	assert (ent_rlist_len (rlist) == 0);

	size_t ranges_len;

	struct ent_rlist_range const * ranges =
	    ent_rlist_ranges (rlist, &ranges_len);

	assert (ranges_len == 0);
	assert (ranges == NULL);

	ent_rlist_free (rlist);
	return 0;
}

static int
null_rlist_sets_einval()
{
	errno = 0;
	assert (ent_rlist_len (NULL) == 0);
	assert (errno == EINVAL);

	errno = 0;
	ent_rlist_append (NULL, 1, 2);
	assert (errno == EINVAL);

	errno = 0;
	ent_rlist_append_inverse (NULL, NULL, 2);
	assert (errno == EINVAL);

	errno = 0;
	size_t len;
	ent_rlist_ranges (NULL, &len);
	assert (errno == EINVAL);

	errno = 0;
	ent_rlist_len (NULL);
	assert (errno == EINVAL);

	errno = 0;
	ent_rlist_free (NULL);
	assert (errno == EINVAL);

	return 0;
}

static int
inverted_range_is_invalid()
{
	struct ent_rlist * rlist = ent_rlist_alloc();

	assert (ent_rlist_append (rlist, 2, 1) == -1);
	assert (errno == EINVAL);
	ent_rlist_free (rlist);

	return 0;
}

static int
rlist_general_tests()
{
	struct ent_rlist * rlist = ent_rlist_alloc();

	if (rlist == NULL)
	{
		return -1;
	}

	assert (rlist != NULL);

	// Append range to empty rlist
	if (ent_rlist_append (rlist, 2, 5) == -1)
	{
		ent_rlist_free (rlist);
		return -1;
	}

	size_t ranges_len;
	struct ent_rlist_range const * ranges = ent_rlist_ranges (rlist, &ranges_len);
	assert (ranges_len == 1);
	assert (ranges[0].begin == 2);
	assert (ranges[0].end == 5);

	// Cannot insert prior range
	errno = 0;
	assert (ent_rlist_append (rlist, 0, 1) == -1);
	assert (errno = EINVAL);
	errno = 0;
	assert (ent_rlist_append (rlist, 0, 4) == -1);
	assert (errno = EINVAL);

	// Overlapping range is ok (?)
	if (ent_rlist_append (rlist, 3, 4) == -1)
	{
		ent_rlist_free (rlist);
		return -1;
	}
	ranges = ent_rlist_ranges (rlist, &ranges_len);
	assert (ranges_len == 1);
	assert (ranges[0].begin == 2);
	assert (ranges[0].end == 5);

	// Extend existing range
	if (ent_rlist_append (rlist, 5, 7) == -1)
	{
		ent_rlist_free (rlist);
		return -1;
	}
	ranges = ent_rlist_ranges (rlist, &ranges_len);
	assert (ranges_len == 1);
	assert (ranges[0].begin == 2);
	assert (ranges[0].end == 7);

	// Additional range
	if (ent_rlist_append (rlist, 9, 10) == -1)
	{
		ent_rlist_free (rlist);
		return -1;
	}
	ranges = ent_rlist_ranges (rlist, &ranges_len);
	assert (ranges_len == 2);
	assert (ranges[0].begin == 2);
	assert (ranges[0].end == 7);
	assert (ranges[1].begin == 9);
	assert (ranges[1].end == 10);

	ent_rlist_free (rlist);

	return 0;
}

void
rlist_test()
{
	assert (new_rlist_is_empty() == 0);
	assert (null_rlist_sets_einval() == 0);
	assert (inverted_range_is_invalid() == 0);

	size_t zero = ent_alloc_count();
	assert (rlist_general_tests() == 0);
	size_t used = ent_alloc_count() - zero;

	for (size_t i = 1; i <= used; ++i)
	{
		zero = ent_alloc_count();
		ent_alloc_artificial_fail (zero + i);
		assert (rlist_general_tests() == -1);
		assert (errno == ENOMEM);
	}
}
