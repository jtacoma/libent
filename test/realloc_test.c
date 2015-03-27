#include "test/ent_test.h"
#include "ent_internal.h"

void realloc_test()
{
	int16_t * p = NULL;
	p = ent_realloc_array (p, 3, true);
	assert_true (p != NULL);
	for (int i = 0; i < 3; ++i)
	{
		assert_true (p[i] == 0);
	}
	size_t * size = (void*)p;
	size--;
	assert_true (*size == 3 * sizeof (int16_t));
	ent_realloc_free (p);

	p = ent_realloc (NULL, SIZE_MAX / 4, false) ;
	assert_true (p == NULL);
}
