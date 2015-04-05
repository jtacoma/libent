#include "test/ent_test.h"
#include "model.h"
#include "table.h"

void model_test()
{
	struct ent_model * m = ent_model_alloc();
	assert (m != NULL);
	assert (ent_model_has (m, "items") == false);
	assert (ent_model_has (m, NULL) == false);
	assert (ent_model_has (NULL, "items") == false);

	struct ent_table * items = ent_model_get_const (m, "items");
	assert (items != NULL);
	ent_table_free (items);

	assert (ent_model_has (m, "items"));
	assert (ent_model_has (m, NULL) == false);
	assert (ent_model_has (NULL, "items") == false);
	assert (ent_model_get_const (m, NULL) == NULL);
	assert (ent_model_get_const (NULL, "items") == NULL);

	ent_model_free (m);
	ent_model_free (NULL);
}
