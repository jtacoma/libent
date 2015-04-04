#ifndef CREATOR_H
#define CREATOR_H

#include "ent.h"

struct creator;

struct creator *
creator_alloc();

void
creator_free();

int
creator_bind_model (struct creator * creator, struct ent_model * model);

int
creator_execute (struct creator * creator, size_t appending);

#endif//CREATOR_H
