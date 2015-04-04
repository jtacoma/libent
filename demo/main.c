#include "ent.h"

#include "creator.h"
#include "printer.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>

int main()
{
	struct ent_model * model = ent_model_alloc();
	assert (model);

	struct creator * creator = creator_alloc();
	assert (creator);

	assert (creator_bind_model (creator, model) == 0);

	for (size_t i = 1; i <= 16; ++i)
	{
		assert (creator_execute (creator, i) == 0);
	}

	creator_free (creator);

	print (model);

	ent_model_free (model);
}
