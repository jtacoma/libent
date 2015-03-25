#include "ent.h"
#include "ent_internal.h"

#include <string.h>
#include <stdlib.h>

struct ent_typeinfo builtin_types[] =
{
	{
		.cname = "bytes",
		.elem = ENT_TYPE_UTF8,
		.elemsize = sizeof(struct ent_bytes),
		.dimensions = 1,
		.n = 1,
		.clear = ent_bytes_clear,
	},
};

#define builtin_types_len \
	((int)(sizeof(builtin_types)/sizeof(*builtin_types)))

int ent_typeinfo_parse(struct ent_typeinfo * typeinfo, char const * name)
{
	for (size_t i = 0; i < builtin_types_len; ++i)
	{
		if (strcmp(builtin_types[i].cname, name) == 0)
		{
			*typeinfo = builtin_types[i];
			return 0;
		}
	}
	return -1; // no such type
}

bool ent_typeinfo_equal(struct ent_typeinfo const * typeinfo,
                        struct ent_typeinfo const * other)
{
	return typeinfo == other ||
	       strcmp(typeinfo->cname, other->cname) == 0;
}

size_t ent_typeinfo_width(struct ent_typeinfo const * t)
{
	return t->elemsize * t->dimensions * t->n;
}

