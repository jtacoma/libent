#include "test/ent_test.h"
#include "model.h"
#include "table.h"

void model_test()
{
	struct ent_model * m = ent_model_alloc();
	assert_true (m != NULL);
	assert_true (ent_model_has (m, "items") == false);
	assert_true (ent_model_has (m, NULL) == false);
	assert_true (ent_model_has (NULL, "items") == false);

	struct ent_table * items = ent_model_get (m, "items");
	assert_true (items != NULL);
	ent_table_free (items);

	assert_true (ent_model_has (m, "items"));
	assert_true (ent_model_has (m, NULL) == false);
	assert_true (ent_model_has (NULL, "items") == false);
	assert_true (ent_model_get (m, NULL) == NULL);
	assert_true (ent_model_get (NULL, "items") == NULL);

	ent_model_free (m);
	ent_model_free (NULL);
}
