#include "test/ent_test.h"
#include "test/ent_all_tests.h"

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

int
main()
{
	size_t test_count = sizeof (all_void_tests) / sizeof (*all_void_tests)
	                    + sizeof (all_mem_tests) / sizeof (*all_mem_tests);

	printf ("running %lu tests...\n", test_count);

	for (size_t i = 0; i < sizeof (all_void_tests) / sizeof (*all_void_tests); ++i)
	{
		printf ("  %s: ", all_void_tests[i].name);
		all_void_tests[i].function();
		printf ("verified.\n");
	}

	for (size_t i = 0; i < sizeof (all_mem_tests) / sizeof (*all_mem_tests); ++i)
	{
		printf ("  %s: ", all_mem_tests[i].name);
		size_t zero = ent_alloc_count();
		assert (all_mem_tests[i].function() == 0);
		size_t used = ent_alloc_count() - zero;
		printf ("%lu allocations, ", used);

		for (size_t m = 1; m <= used; ++m)
		{
			ent_alloc_artificial_fail (ent_alloc_count() + m);
			errno = 0;
			assert (all_mem_tests[i].function() == -1);
			assert (errno == ENOMEM);
		}

		printf ("verified.\n");
	}

	printf ("all %lu tests passed.\n", test_count);
}
