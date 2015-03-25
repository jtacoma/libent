#include "test/ent_test.h"
#include "ent_internal.h"

void
range_test()
{
	struct ent_range * range = ent_range_alloc();
	assert_true(range != NULL);
	assert_true(ent_range_len(range) == 0);

	// Initial range is empty
	size_t chunks_len = 1;
	struct ent_range_chunk const * chunks =
	    ent_range_chunks(range, &chunks_len);
	assert_true(chunks_len == 0);
	assert_true(chunks == NULL);

	// Inverted chunk is invalid
	assert_true(ent_range_append(range, 2, 1) == -1);

	// Append chunk to empty range
	assert_true(ent_range_append(range, 2, 5) == 0);
	chunks = ent_range_chunks(range, &chunks_len);
	assert_true(chunks_len == 1);
	assert_true(chunks[0].begin == 2);
	assert_true(chunks[0].end == 5);

	// Cannot insert prior chunk
	assert_true(ent_range_append(range, 0, 1) == -1);
	assert_true(ent_range_append(range, 0, 4) == -1);

	// Overlapping chunk is ok (?)
	assert_true(ent_range_append(range, 3, 4) == 0);
	chunks = ent_range_chunks(range, &chunks_len);
	assert_true(chunks_len == 1);
	assert_true(chunks[0].begin == 2);
	assert_true(chunks[0].end == 5);

	// Extend existing chunk
	assert_true(ent_range_append(range, 5, 7) == 0);
	chunks = ent_range_chunks(range, &chunks_len);
	assert_true(chunks_len == 1);
	assert_true(chunks[0].begin == 2);
	assert_true(chunks[0].end == 7);

	// Additional chunk
	assert_true(ent_range_append(range, 9, 10) == 0);
	chunks = ent_range_chunks(range, &chunks_len);
	assert_true(chunks_len == 2);
	assert_true(chunks[0].begin == 2);
	assert_true(chunks[0].end == 7);
	assert_true(chunks[1].begin == 9);
	assert_true(chunks[1].end == 10);

	ent_range_free(range);
}
