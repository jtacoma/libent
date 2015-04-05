#ifndef ENTLIB_COLUMN_H
#define ENTLIB_COLUMN_H

struct ent_array;

struct ent_array *
ent_array_alloc (
    size_t width);

struct ent_array *
ent_array_cpy_alloc (
    struct ent_array const * src);

void
ent_array_free (
    struct ent_array * a);

void const *
ent_array_get_const (
    struct ent_array const * a);

size_t
ent_array_len (
    struct ent_array const * a);

size_t
ent_array_width (
    struct ent_array const * a);

void *
ent_array_get (
    struct ent_array * a);

int
ent_array_set_len (
    struct ent_array * a,
    size_t add);

#define ent_array_typed(T) \
	struct ent_ ## T ## _array; \
	static inline size_t ent_ ## T ## _array_width() \
	{ \
		return sizeof(T); \
	} \
	static inline struct ent_ ## T ## _array * ent_ ## T ## _array_alloc() \
	{ \
		return (struct ent_ ## T ## _array *) ent_array_alloc (sizeof (T)); \
	} \
	static inline void ent_ ## T ## _array_free (struct ent_ ## T ## _array * array) \
	{ \
		ent_array_free ((struct ent_array *) array); \
	} \
	static inline T const * ent_ ## T ## _array_get_const (struct ent_ ## T ## _array const * array) \
	{ \
		return ent_array_get_const ((struct ent_array const *) array); \
	} \
	static inline size_t ent_ ## T ## _array_len (struct ent_ ## T ## _array const * array) \
	{ \
		return ent_array_len ((struct ent_array const *) array); \
	} \
	static inline T * ent_ ## T ## _array_get (struct ent_ ## T ## _array * array) \
	{ \
		return ent_array_get ((struct ent_array *) array); \
	} \
	static inline int ent_ ## T ## _array_set_len (struct ent_ ## T ## _array * array, size_t len) \
	{ \
		return ent_array_set_len ((struct ent_array *) array, len); \
	} \
	struct ent_ ## T ## _array { uint8_t _; uint8_t __[]; }

ent_array_typed (char);

#endif//Eent_ ## T ## _arrayTLIB_COLUMent_ ## T ## _array_H
