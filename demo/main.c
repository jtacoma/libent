#include "ent.h"

#include "creator.h"
#include "printer.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	struct ent_table * entities = ent_table_alloc();
	assert (entities);

	struct creator * creator = creator_alloc();
	assert (creator);

	assert (creator_bind (creator, entities) == 0);

	for (size_t i = 1; i <= 16; ++i)
	{
		assert (creator_execute (creator, i) == 0);
	}

	creator_free (creator);

	print (entities);

	ent_table_free (entities);
}
