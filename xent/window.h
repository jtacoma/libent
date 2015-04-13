#ifndef WINDOW_H
#define WINDOW_H

struct window;

struct window *
window_alloc (
    struct ent_table * entities);

void
window_free (
    struct window * w);

int
window_setup (
    struct window * w);

int
window_teardown (
    struct window * w);

int
window_paint (
    struct window * w);

int
window_input (
    struct window * w);

bool
window_stopping (
    struct window const * w);

#endif//WINDOW_H
