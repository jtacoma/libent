#ifndef LIBENT_ALLOC_H
#define LIBENT_ALLOC_H

#include <stddef.h>

int ent_alloc (void ** ptr, size_t size);

size_t ent_alloc_count();

void ent_alloc_artificial_fail (size_t at_count);

#endif//LIBENT_ALLOC_H