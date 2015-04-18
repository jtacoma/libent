#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ent.h"

#include "alloc.h"
#include "array.h"
#include "lock.h"
#include "rlist.h"
#include "table.h"

void true_or_exit (bool value, char const * file, int line,
                   char const * literal);

#define assert(expr) \
	true_or_exit((expr), __FILE__, __LINE__, "false: " #expr)

extern struct ent_column * test_int8column;
extern struct ent_column * test_int16column;
extern struct ent_column * test_int32column;
extern struct ent_column * test_int64column;
extern struct ent_column * test_float32column;
extern struct ent_column * test_float64column;
extern struct ent_column * test_ptrcolumn;
