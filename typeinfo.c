#include "ent.h"
#include "typeinfo.h"

#include <string.h>
#include <stdlib.h>

struct ent_typeinfo builtin_types[] =
{
	{
		.cname = "bytes",
		.elem = ENT_TYPE_BYTES,
		.elemsize = sizeof (void*),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "int8",
		.elem = ENT_TYPE_INT8,
		.elemsize = sizeof (int8_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "int16",
		.elem = ENT_TYPE_INT16,
		.elemsize = sizeof (int16_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "int32",
		.elem = ENT_TYPE_INT32,
		.elemsize = sizeof (int32_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "int64",
		.elem = ENT_TYPE_INT64,
		.elemsize = sizeof (int64_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "uint8",
		.elem = ENT_TYPE_UINT8,
		.elemsize = sizeof (uint8_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "uint16",
		.elem = ENT_TYPE_UINT16,
		.elemsize = sizeof (uint16_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "uint32",
		.elem = ENT_TYPE_UINT32,
		.elemsize = sizeof (uint32_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "uint64",
		.elem = ENT_TYPE_UINT64,
		.elemsize = sizeof (uint64_t),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "float32",
		.elem = ENT_TYPE_FLOAT32,
		.elemsize = sizeof (float),
		.dimensions = 1,
		.n = 1,
	},
	{
		.cname = "float64",
		.elem = ENT_TYPE_FLOAT64,
		.elemsize = sizeof (double),
		.dimensions = 1,
		.n = 1,
	},
};

#define builtin_types_len \
	((int)(sizeof(builtin_types)/sizeof(*builtin_types)))

int ent_typeinfo_parse (struct ent_typeinfo * typeinfo, char const * name)
{
	for (size_t i = 0; i < builtin_types_len; ++i)
	{
		if (strcmp (builtin_types[i].cname, name) == 0)
		{
			*typeinfo = builtin_types[i];
			return 0;
		}
	}

	return -1;
}

bool ent_typeinfo_equal (struct ent_typeinfo const * typeinfo,
                         struct ent_typeinfo const * other)
{
	return typeinfo == other ||
	       strcmp (typeinfo->cname, other->cname) == 0;
}

char const * ent_typeinfo_name (struct ent_typeinfo const * t)
{
	return t->cname;
}

size_t ent_typeinfo_width (struct ent_typeinfo const * t)
{
	return t->elemsize * t->dimensions * t->n;
}

