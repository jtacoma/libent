#ifndef BENCH_H
#define BENCH_H

#include <stdint.h>

typedef uint_fast32_t bench_count;

typedef void (*bench_func) (bench_count);

#endif//BENCH_H
