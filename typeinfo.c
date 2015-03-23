#include "ent.h"
#include "ent_internal.h"

#include <string.h>
#include <stdlib.h>

struct ent_typeinfo
{
	char const *cname;
	enum ent_elemtype elem;
	size_t elemsize;
	size_t dimensions;
	size_t n;
};

struct ent_typeinfo builtin_types[] =
{
	{
		.cname = "utf8",
		.elem = ENT_TYPE_UTF8,
		.elemsize = sizeof(char *),
		.dimensions = 1,
		.n = 1,
	},
};

#define builtin_types_len ((int)(sizeof(builtin_types)/sizeof(*builtin_types)))

struct ent_typeinfo const * ent_typeinfo_from_name(char const * name)
{
	for (size_t i = 0; i < builtin_types_len; ++i)
	{
		if (strcmp(builtin_types[i].cname, name) == 0)
		{
			return &builtin_types[i];
		}
	}
	return NULL;
}

size_t ent_typeinfo_width(struct ent_typeinfo const * t)
{
	return t->elemsize * t->dimensions * t->n;
}
