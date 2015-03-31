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
struct ent_session;

struct ent_model *
ent_model_alloc();

void
ent_model_free (
    struct ent_model * m);

struct ent_session *
ent_session_alloc (
    struct ent_model * m);

void
ent_session_free (
    struct ent_session * s);

int
ent_session_table (
    struct ent_session * s,
    char const * table_name,
    char const * mode);

int
ent_session_column (
    struct ent_session * s,
    int table,
    char const * column_name,
    size_t width,
    char const * mode);

int
ent_session_lock (
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

void const *
ent_session_column_read (
    struct ent_session * s,
    int table,
    int column);

size_t
ent_session_column_width (
    struct ent_session * s,
    int column);

void *
ent_session_column_write (
    struct ent_session * s,
    int table,
    int column);

#endif//LIBENT_ENT_H
