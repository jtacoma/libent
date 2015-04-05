#ifndef LIBENT_ENT_H
#define LIBENT_ENT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct ent_rlist;

struct ent_rlist *
ent_rlist_alloc();

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

struct ent_model;
struct ent_processor;
struct ent_session;
typedef int (* ent_procfunc) (struct ent_session *, void *);

struct ent_model *
ent_model_alloc();

void
ent_model_free (
    struct ent_model * m);

struct ent_processor *
ent_processor_alloc (
    struct ent_model * m);

void
ent_processor_free (
    struct ent_processor * p);

int
ent_processor_use_table (
    struct ent_processor * p,
    char const * table_name,
    char const * mode);

int
ent_processor_use_column (
    struct ent_processor * p,
    int table,
    char const * column_name,
    size_t width,
    char const * mode);

struct ent_session *
ent_session_alloc (
    struct ent_processor * p);

void
ent_session_free (
    struct ent_session * s);

size_t
ent_session_table_len (
    struct ent_session * s,
    int table);

int
ent_session_table_insert (
    struct ent_session * s,
    int table,
    size_t add_size);

int
ent_session_table_delete (
    struct ent_session * s,
    int table,
    struct ent_rlist const * rlist);

void const *
ent_session_column_get_const (
    struct ent_session * s,
    int table,
    int column);

size_t
ent_session_column_width (
    struct ent_session * s,
    int column);

void *
ent_session_column_get (
    struct ent_session * s,
    int table,
    int column);

int
ent_session_commit (
    struct ent_session * s);

#endif//LIBENT_ENT_H
