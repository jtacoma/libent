#ifndef ENTLIB_RLIST_H
#define ENTLIB_RLIST_H

struct ent_rlist_range
{
	size_t begin;
	size_t end;
};

struct ent_rlist_range const *
ent_rlist_ranges (struct ent_rlist const * rlist,
                  size_t *len);

#endif//ENTLIB_RLIST_H
