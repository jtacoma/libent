#include "ent.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int
main ()
{
	struct ent_range * range = ent_range_alloc ();
	ent_range_append (range, 0, 2);
	ent_range_append (range, 2, 4);
	assert (ent_range_len (range) == 4);

	struct ent_column * names = ent_column_alloc (range, "name", "bytes");
	assert (names != NULL);

	struct ent_bytes * dst = ent_column_ref (names);
	assert (dst != NULL);

	char const * src[] = { "Lana", "Archer", "Cyril" };
	for (size_t i = 0; i < sizeof(src) / sizeof(*src); ++i, ++dst)
	{
		ent_bytes_reset (dst, src[i], strlen(src[i]) + 1);

		size_t len;
		char const * result = ent_bytes_get (dst, &len);
		assert (len == strlen (src[i]) + 1);
		assert (strcmp (result, src[i]) == 0);
	}

	ent_bytes_reset (dst, "Randy", 6);
	size_t randy_len;
	assert (strcmp ("Randy", ent_bytes_get (dst, &randy_len)) == 0);
	assert (randy_len == 6);

	struct ent_range * delete = ent_range_alloc ();
	assert (ent_range_append (delete, 1, 2) == 0);
	assert (ent_range_delete (range, delete) == 0);
	assert (ent_range_len (range) == 3);
	char const * expected[] = { "Lana", "Cyril", "Randy" };
	struct ent_bytes * actual = ent_column_ref (names);
	for (size_t i = 0; i < 3; ++i)
	{
		size_t len;
		char const * s = ent_bytes_get(&actual[i], &len);
		assert (len == strlen(s) + 1);
		assert (strcmp (s, expected[i]) == 0);
	}
	ent_range_free (delete);

	ent_column_free (names);
	ent_range_free (range);
}
