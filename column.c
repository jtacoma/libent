#include "ent.h"
#include "ent_internal.h"

#include <stdlib.h>
#include <string.h>

struct ent_column
{
	char * name;
	void * start;
	struct ent_typeinfo type;
	size_t len;
};

struct ent_column *
ent_column_alloc (struct ent_range * range, char const * name, char const * type)
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
	struct ent_column * column = calloc(1, sizeof(*column));
	if (column == NULL)
	{
		free(newname);
		return NULL;
	}
	column->name = newname;
	if (ent_typeinfo_parse( &column->type, type) == -1)
	{
		free (column);
		free (newname);
		return NULL;
	}

	column->len = ent_range_len(range);
	if (column->len > 0)
	{
		column->start = calloc(column->len, ent_typeinfo_width(&column->type));
		if (column->start == NULL)
		{
			free (column);
			free (newname);
			return NULL;
		}
	}

	return column;
}

void * ent_column_ref (struct ent_column * c)
{
	return c->start;
}

int ent_column_set(struct ent_column * c, void * start, size_t len)
{
	if (c->start != NULL)
	{
		free(c->start);
	}
	c->start = start;
	c->len = len;
	return 0;
}

void ent_column_free(struct ent_column * c)
{
	if (c != NULL)
	{
		if (c->start != NULL)
		{
			free(c->start);
		}
		free(c);
	}
}
