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

int
main()
{
	size_t test_count = sizeof (all_mem_tests) / sizeof (*all_mem_tests);

	printf ("running %lu tests...\n", test_count);

	for (size_t i = 0;
	        i < sizeof (all_mem_tests) / sizeof (*all_mem_tests);
	        ++i)
	{
		printf ("  %s: ", all_mem_tests[i].name);
		size_t prev_fail_at = ent_alloc_fail_at();
		size_t zero = ent_alloc_count();
		assert (all_mem_tests[i].function() == 0);
		size_t used = ent_alloc_count() - zero;

		// Only apply memory allocation stress if this test does not
		// stress memory allocation on its own.
		if (ent_alloc_fail_at() == prev_fail_at)
		{
			printf ("(%lu allocations) ", used);
			for (size_t m = 1; m <= used; ++m)
			{
				ent_alloc_set_fail_at (ent_alloc_count() + m);
				errno = 0;
				assert (all_mem_tests[i].function() == -1);
				assert (errno == ENOMEM);
			}
		}

		printf ("verified.\n");
	}

	printf ("all %lu tests passed.\n", test_count);
}
