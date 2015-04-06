#ifndef ENTLIB_RLIST_H
#define ENTLIB_RLIST_H

struct ent_rlist_range
{
	size_t begin;
	size_t end;
};

int
ent_rlist_append_inverse (
    struct ent_rlist * dst,
    struct ent_rlist const * src,
    size_t len);

struct ent_rlist_range const *
ent_rlist_ranges (
    struct ent_rlist const * rlist,
    size_t *len);

#endif//ENTLIB_RLIST_H
