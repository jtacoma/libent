#ifndef LIBENT_COLUMN_H
#define LIBENT_COLUMN_H

struct ent_column
{
	int id;
	size_t width;
};

int
ent_column_id (
    struct ent_column const * column);

#endif//LIBENT_COLUMN_H
