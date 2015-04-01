#include "test/ent_test.h"

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
	// Roughly in dependency order so that failures in the most
	// depended-upon modules will be discovered first.
	rlist_test();
	printf ("rlist: passed\n");
	array_test();
	printf ("array: passed\n");
	table_test();
	printf ("table: passed\n");
	model_test();
	printf ("model: passed\n");
	session_test();
	printf ("session: passed\n");
}
