#include "ent.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct test_context
{
	int expected_errno;
};

void true_or_exit (bool value, char const * file, int line,
                   char const * literal);

#define assert(expr) \
	true_or_exit((expr), __FILE__, __LINE__, "false: " #expr)

void alloc_test();
void array_test();
void rlist_test();
void session_test();
void table_test();
