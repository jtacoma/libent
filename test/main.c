#include "ent_test.h"
#include "ent_all_tests.h"

void true_or_exit (bool value, char const * file, int line,
                   char const * literal)
{
	if (!value)
	{
		fprintf (stderr, "%s:%d: %s\n",
		         file, line, literal);
		exit (1);
	}
}

struct ent_column * test_int8column;
struct ent_column * test_int16column;
struct ent_column * test_int32column;
struct ent_column * test_int64column;
struct ent_column * test_float32column;
struct ent_column * test_float64column;
struct ent_column * test_ptrcolumn;

int
main ()
{
	test_int8column = ent_column_alloc (sizeof (int8_t));
	test_int16column = ent_column_alloc (sizeof (int16_t));
	test_int32column = ent_column_alloc (sizeof (int32_t));
	test_int64column = ent_column_alloc (sizeof (int64_t));
	test_float32column = ent_column_alloc (sizeof (float));
	test_float64column = ent_column_alloc (sizeof (double));
	test_ptrcolumn = ent_column_alloc (sizeof (void*));

	size_t test_count = sizeof (all_mem_tests) / sizeof (*all_mem_tests);

	printf ("running %lu tests...\n", test_count);

	for (size_t i = 0;
	        i < sizeof (all_mem_tests) / sizeof (*all_mem_tests);
	        ++i)
	{
		printf ("  %s: ", all_mem_tests[i].name);
		size_t prev_fail_at = ent_alloc_fail_at ();
		size_t zero = ent_alloc_count ();
		assert (all_mem_tests[i].function () == 0);
		size_t used = ent_alloc_count () - zero;

		// Only apply memory allocation stress if this test does not
		// stress memory allocation on its own.
		if (ent_alloc_fail_at () == prev_fail_at)
		{
			printf ("(%lu allocations) ", used);
			for (size_t m = 1; m <= used; ++m)
			{
				ent_alloc_set_fail_at (ent_alloc_count () + m);
				errno = 0;
				assert (all_mem_tests[i].function () == -1);
				assert (errno == ENOMEM);
			}
		}

		printf ("verified.\n");
	}

	printf ("all %lu tests passed.\n", test_count);

	ent_column_free (test_ptrcolumn);
	ent_column_free (test_float64column);
	ent_column_free (test_float32column);
	ent_column_free (test_int64column);
	ent_column_free (test_int32column);
	ent_column_free (test_int16column);
	ent_column_free (test_int8column);
}
