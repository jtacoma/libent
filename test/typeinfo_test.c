#include "test/ent_test.h"

#include "ent_internal.h"

void
typeinfo_test()
{
	struct ent_typeinfo typeinfo;
	assert_true(ent_typeinfo_parse(&typeinfo, "unknown") == -1);
}
