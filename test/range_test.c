#include "test/ent_test.h"
#include "ent_internal.h"

void
rlist_test()
{
	struct ent_rlist * rlist = ent_rlist_alloc();
	assert_true(rlist != NULL);
	assert_true(ent_rlist_len(rlist) == 0);

	// Initial rlist is empty
	size_t ranges_len = 1;
	struct ent_rlist_range const * ranges =
	    ent_rlist_ranges(rlist, &ranges_len);
	assert_true(ranges_len == 0);
	assert_true(ranges == NULL);

	// Inverted range is invalid
	assert_true(ent_rlist_append(rlist, 2, 1) == -1);

	// Append range to empty rlist
	assert_true(ent_rlist_append(rlist, 2, 5) == 0);
	ranges = ent_rlist_ranges(rlist, &ranges_len);
	assert_true(ranges_len == 1);
	assert_true(ranges[0].begin == 2);
	assert_true(ranges[0].end == 5);

	// Cannot insert prior range
	assert_true(ent_rlist_append(rlist, 0, 1) == -1);
	assert_true(ent_rlist_append(rlist, 0, 4) == -1);

	// Overlapping range is ok (?)
	assert_true(ent_rlist_append(rlist, 3, 4) == 0);
	ranges = ent_rlist_ranges(rlist, &ranges_len);
	assert_true(ranges_len == 1);
	assert_true(ranges[0].begin == 2);
	assert_true(ranges[0].end == 5);

	// Extend existing range
	assert_true(ent_rlist_append(rlist, 5, 7) == 0);
	ranges = ent_rlist_ranges(rlist, &ranges_len);
	assert_true(ranges_len == 1);
	assert_true(ranges[0].begin == 2);
	assert_true(ranges[0].end == 7);

	// Additional range
	assert_true(ent_rlist_append(rlist, 9, 10) == 0);
	ranges = ent_rlist_ranges(rlist, &ranges_len);
	assert_true(ranges_len == 2);
	assert_true(ranges[0].begin == 2);
	assert_true(ranges[0].end == 7);
	assert_true(ranges[1].begin == 9);
	assert_true(ranges[1].end == 10);

	ent_rlist_free(rlist);
}
