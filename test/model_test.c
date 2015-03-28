#include "test/ent_test.h"

void model_test()
{
	struct ent_model * m = ent_model_alloc();
	assert_true (m != NULL);
	assert_true (ent_model_has (m, "items") == false);

	struct ent_table * items = ent_model_get (m, "items");
	assert_true (items == NULL);
	assert_true (ent_model_has (m, "items"));

	ent_model_free (m);
}
