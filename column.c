#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	char * name;
	void * start;
	struct ent_typeinfo const * type;
	size_t n;
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
	struct ent_column column =
	{
		.name = newname,
		.start = NULL,
		.type = type,
		.n = 0,
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

void ent_column_start (struct ent_column * c, void** start, size_t * n)
{
	*start = c->start;
	*n = c->n;
}

struct ent_typeinfo const * ent_column_type (struct ent_column * c)
{
	return c->type;
}

int ent_column_set(struct ent_column * c, void * start, size_t n)
{
	if (c->start != NULL)
	{
		free(c->start);
	}
	c->start = start;
	c->n = n;
	return 0;
}

void ent_column_free(struct ent_column * c)
{
	// TODO
	(void)c;
}
