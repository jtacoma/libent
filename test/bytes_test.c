#include "test/ent_test.h"
#include "ent_internal.h"

void bytes_test()
{
	struct ent_bytes bytes = {0};

	size_t len = 1;
	void const * ptr = ent_bytes_get(&bytes, &len);
	assert_true(len == 0);
	assert_true(ptr == NULL);

	assert_true(ent_bytes_reset(&bytes, NULL, 0) == 0);
	ptr = ent_bytes_get(&bytes, &len);
	assert_true(len == 0);
	assert_true(ptr == NULL);

	assert_true(ent_bytes_reset(&bytes, "Hello", 6) == 0);
	ptr = ent_bytes_get(&bytes, &len);
	assert_true(len == 6);
	assert_true(strcmp(ptr, "Hello") == 0);

	assert_true(ent_bytes_reset(&bytes, NULL, 0) == 0);
	ptr = ent_bytes_get(&bytes, &len);
	assert_true(len == 0);
	assert_true(ptr == NULL);
}
