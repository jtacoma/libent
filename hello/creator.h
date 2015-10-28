#ifndef CREATOR_H
#define CREATOR_H

#include "ent.h"

struct creator;

struct creator *
creator_alloc ();

void
creator_free ();

int
creator_bind (struct creator * creator, struct ent_table * entities);

int
creator_execute (struct creator * creator, size_t appending);

#endif//CREATOR_H
