#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <ent.h>

#include "xent.h"

struct ent_column * input_type_column;
struct ent_column * input_id_column;
struct ent_column * pos_column;
struct ent_column * age_column;
struct ent_column * dead_column;

void
xent_init_columns()
{
	input_type_column = ent_column_alloc (sizeof (enum input_type));
	input_id_column = ent_column_alloc (sizeof (input_id));
	pos_column = ent_column_alloc (sizeof (length_xy));
	age_column = ent_column_alloc (sizeof (float));
	dead_column = ent_column_alloc (sizeof (uint8_t));
}
