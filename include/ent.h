#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

struct ent_rlist;

struct ent_rlist *
ent_rlist_alloc (void);

void
ent_rlist_free (
    struct ent_rlist * rlist);

size_t
ent_rlist_len (
    struct ent_rlist const * rlist);

int
ent_rlist_append (
    struct ent_rlist * rlist,
    size_t begin,
    size_t end);

int
ent_rlist_delete (
    struct ent_rlist * rlist,
    struct ent_rlist const * src);

int
ent_rlist_select (
    struct ent_rlist const * rlist,
    void * dst,
    void const * src,
    size_t width);

struct ent_column;

struct ent_column *
ent_column_alloc (
    size_t width);

void
ent_column_free (
    struct ent_column * column);

size_t
ent_column_width (
    struct ent_column const * column);

struct ent_table;

struct ent_table *
ent_table_alloc (void);

void
ent_table_free (
    struct ent_table * table);

struct ent_lock;

struct ent_lock *
ent_lock_alloc (void);

void
ent_lock_free (
    struct ent_lock * p);

int
ent_lock_for_insert (
    struct ent_lock * p,
    struct ent_table * table_name);

int
ent_lock_for_delete (
    struct ent_lock * p,
    struct ent_table * table_name);

int
ent_lock_for_update (
    struct ent_lock * p,
    struct ent_table * table,
    struct ent_column const * column);

int
ent_lock_for_select (
    struct ent_lock * p,
    struct ent_table * table,
    struct ent_column const * column);

struct ent_session;

struct ent_session *
ent_session_alloc (
    struct ent_lock * p);

void
ent_session_free (
    struct ent_session * s);

size_t
ent_session_table_len (
    struct ent_session * s,
    struct ent_table * table);

struct ent_table *
ent_session_insert (
    struct ent_session * s,
    struct ent_table * table,
    size_t add_size);

int
ent_session_delete (
    struct ent_session * s,
    struct ent_table * table,
    struct ent_rlist const * rlist);

void const *
ent_session_select (
    struct ent_session * s,
    struct ent_table * table,
    struct ent_column const * column);

void *
ent_session_update (
    struct ent_session * s,
    struct ent_table * table,
    struct ent_column const * column);

int
ent_session_commit (
    struct ent_session * s);

#endif//LIBENT_ENT_H
