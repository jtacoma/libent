#ifndef LIBENT_ALLOC_H
#define LIBENT_ALLOC_H

int ent_alloc (void ** ptr, size_t size);

size_t ent_alloc_count (void);

size_t ent_alloc_fail_at (void);

void ent_alloc_set_fail_at (size_t at_count);

#endif//LIBENT_ALLOC_H
