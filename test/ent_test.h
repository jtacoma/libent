#include "ent.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void true_or_exit (bool value, char const * file, int line,
                   char const * literal);

#define assert_true(expr) \
	true_or_exit((expr), __FILE__, __LINE__, "false: " #expr)

void column_test();
void rlist_test();
void typeinfo_test();
