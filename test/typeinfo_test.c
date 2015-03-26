#include "test/ent_test.h"

#include "ent_internal.h"

void
typeinfo_test()
{
	struct ent_typeinfo typeinfo;
	assert_true(ent_typeinfo_parse(&typeinfo, "unknown") == -1);
	assert_true(ent_typeinfo_parse(&typeinfo, "bytes") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(void*));
	assert_true(ent_typeinfo_parse(&typeinfo, "int8") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(int8_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "int16") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(int16_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "int32") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(int32_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "int64") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(int64_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "uint8") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(uint8_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "uint16") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(uint16_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "uint32") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(uint32_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "uint64") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == sizeof(uint64_t));
	assert_true(ent_typeinfo_parse(&typeinfo, "float32") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == 4);
	assert_true(ent_typeinfo_parse(&typeinfo, "float64") == 0);
	assert_true(ent_typeinfo_width(&typeinfo) == 8);
}
