#include "ent.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

int
main ()
{
	struct ent_typeinfo const * utf8 = ent_typeinfo_from_name("utf8");
	assert (utf8 != NULL);

	struct ent_column * names = ent_column_alloc ("name", utf8);
	assert (names != NULL);

	struct ent_insert_buffer * new_names = ent_insert_buffer_alloc (names);
	assert (new_names != NULL);
	char const * input[2] = { "Lana", "Archer" };
	assert (ent_insert_buffer_append (new_names, input, 2) == 0);
	assert (ent_insert_buffer_commit (new_names) == 0);
	assert (ent_insert_buffer_append (new_names, input, 2) == -1);
	assert (ent_insert_buffer_commit (new_names) == -1);
	ent_insert_buffer_free(new_names);

	char const ** start = NULL;
	size_t size = 0;
	ent_column_start (names, (void**)&start, &size);
	assert (size == 2);
	assert (strcmp (start[0], "Lana") == 0);
	assert (strcmp (start[1], "Archer") == 0);

	struct ent_update_buffer * changed_names = ent_update_buffer_alloc(names);
	assert(changed_names != NULL);
	char const * changes[1] = { "Randy" };
	assert(ent_update_buffer_set(changed_names, 1, changes, 1) == 0);
	assert(ent_update_buffer_commit(changed_names) == 0);
	assert(ent_update_buffer_set(changed_names, 1, changes, 1) == -1);
	assert(ent_update_buffer_commit(changed_names) == -1);
	ent_update_buffer_free(changed_names);

	start = NULL;
	size = 0;
	ent_column_start (names, (void**)&start, &size);
	assert (size == 2);
	assert (strcmp (start[0], "Lana") == 0);
	assert (strcmp (start[1], "Randy") == 0);

	struct ent_delete_buffer * rm_names = ent_delete_buffer_alloc(names);
	assert(rm_names != NULL);
	assert(ent_delete_buffer_rm(rm_names, 0, 1) == 0);

	ent_column_free(names);
}
