#include "ent.h"
#include "table.h"
#include "array.h"
#include "bench/bench.h"

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

FILE * dump = NULL;

double update (double * a, double * b, double * c, bench_count n)
{
	double result = 0;

	for (bench_count i = 0; i < n; ++i)
	{
		a[i] = i;
		b[i] = i;
	}

	for (bench_count i = 0; i < n; ++i)
	{
		c[i] = a[i] * b[i];
		result += c[i];
	}

	fprintf (dump, "%f\n", result);
	return result;
}

struct bloody_wide
{
	double a;
	double b;
	double c;
	double matrix[9];
	double some_other_stuff[9];
};

void update_wide_structs_bench (bench_count n)
{
	struct bloody_wide * abc = calloc (n, sizeof (*abc));
	double result = 0;

	for (bench_count i = 0; i < n; ++i)
	{
		abc[i].a = i;
		abc[i].b = i;
	}

	for (bench_count i = 0; i < n; ++i)
	{
		abc[i].c = abc[i].a + abc[i].b;
		result += abc[i].c;
	}

	free (abc);
	fprintf (dump, "%f\n", result);
}

struct bloody_narrow
{
	double a;
	double b;
	double c;
};

void update_narrow_structs_bench (bench_count n)
{
	struct bloody_narrow * abc = calloc (n, sizeof (*abc));
	double result = 0;

	for (bench_count i = 0; i < n; ++i)
	{
		abc[i].a = i;
		abc[i].b = i;
	}

	for (bench_count i = 0; i < n; ++i)
	{
		abc[i].c = abc[i].a + abc[i].b;
		result += abc[i].c;
	}

	free (abc);
	fprintf (dump, "%f\n", result);
}

void update_arrays_bench (bench_count n)
{
	double * a = calloc (n , sizeof (double));
	double * b = calloc (n , sizeof (double));
	double * c = calloc (n , sizeof (double));

	update (a, b, c, n);

	free (c);
	free (b);
	free (a);
}

void update_table_bench (bench_count n)
{
	struct ent_table * table = ent_table_alloc();
	ent_table_grow (table, (size_t)n);
	double * a = ent_array_get (ent_table_column (table, "a", 8));
	double * b = ent_array_get (ent_table_column (table, "b", 8));
	double * c = ent_array_get (ent_table_column (table, "c", 8));

	update (a, b, c, n);

	ent_table_free (table);
}

struct bench_info
{
	char const * name;
	bench_func func;
};

void run (bench_func func, long max_ns, bench_count * count, long * elapsed_ns)
{
	for (*count = 1;
	        *count < ((bench_count) - 1) / 2;
	        *count *= 2)
	{
		struct timespec start;
		assert (clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &start) != -1);

		func (*count);

		struct timespec end;
		assert (clock_gettime (CLOCK_PROCESS_CPUTIME_ID, &end) != -1);

		*elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9L
		              + (end.tv_nsec - start.tv_nsec);

		if (*elapsed_ns >= max_ns)
		{
			break;
		}
	}
}

int main()
{
	dump = fopen ("/dev/null", "w");
	long max_ns = 1e9 / 60;
	//max_ns = 1e9;

	struct bench_info benches[] =
	{
		{ "update_arrays", update_arrays_bench },
		{ "update_wide_structs", update_wide_structs_bench },
		{ "update_narrow_structs", update_narrow_structs_bench },
		{ "update_table", update_table_bench },
	};

	for (size_t i = 0; i < sizeof (benches) / sizeof (*benches); ++i)
	{
		bench_count count;
		long elapsed;

		run (benches[i].func, max_ns, &count, &elapsed);

		if (count == 0 || elapsed == 0)
		{
			printf ("%s something went wrong\n", benches[i].name);
			continue;
		}

		printf ("%s %e ops/s averaging %e ns each\n",
		        benches[i].name,
		        1e9 * count / elapsed,
		        (double) elapsed / count);
	}
}
