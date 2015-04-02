#ifndef LIBENT_PROCESSOR_H
#define LIBENT_PROCESSOR_H

#include <stdint.h>

enum mode
{
	MODE_CREATE = 1 << 0,
	MODE_READ = 1 << 1,
	MODE_UPDATE = 1 << 2,
	MODE_DELETE = 1 << 3,
};

struct column_info
{
	char * name;
	size_t width;
};

typedef int (* ent_procfunc) (struct ent_session *, void *);

struct ent_processor *
ent_processor_cpy_alloc (
    struct ent_processor const * processor);

struct ent_table *
ent_processor_table (
    struct ent_processor const * processor,
    int table_id);

struct column_info
ent_processor_column (
    struct ent_processor const * processor,
    int column_id);

#endif//LIBENT_PROCESSOR_H
