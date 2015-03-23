#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	void * start;
	struct ent_typeinfo const * type;
	char * name;
};

struct ent_column *
ent_column_alloc (char const * name, struct ent_typeinfo const * type)
{
	size_t namelen = strlen(name);
	if (namelen > 127)
	{
		return NULL;
	}
	char * newname = malloc(namelen + 1);
	if (newname == NULL)
	{
		return NULL;
	}
	strncpy(newname, name, namelen + 1);
	struct ent_column  column =
	{
		.name = newname,
		.start = NULL,
		.type = type,
	};
	struct ent_column * result = malloc(sizeof(column));
	if (result == NULL)
	{
		free(newname);
		return NULL;
	}
	memcpy(result, &column, sizeof(column));
	return result;
}

void ent_column_start (struct ent_column * c, void** start, size_t * size)
{
	(void)c;
	*start = NULL;
	*size = 0;
}

struct ent_typeinfo const * ent_column_type (struct ent_column * c)
{
	return c->type;
}

void ent_column_free(struct ent_column * c)
{
	// TODO
	(void)c;
}
