#ifndef CLOCK_H
#define CLOCK_H

struct clock;

struct clock *
clock_alloc (void);

void clock_free (
    struct clock * t);

int
clock_sleep (
    struct clock * t);

#endif//CLOCK_H
