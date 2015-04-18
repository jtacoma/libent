#ifndef LIBENT_PROCESSOR_H
#define LIBENT_PROCESSOR_H

enum mode
{
	MODE_CREATE = 1 << 0,
	MODE_READ = 1 << 1,
	MODE_UPDATE = 1 << 2,
	MODE_DELETE = 1 << 3,
};

typedef int (* ent_procfunc) (struct ent_session *, void *);

struct ent_lock *
ent_lock_cpy_alloc (
    struct ent_lock const * lock);

size_t
ent_lock_tables_len (
    struct ent_lock const * lock);

struct ent_table *
ent_lock_table (
    struct ent_lock const * lock,
    int table_id);

#endif//LIBENT_PROCESSOR_H
