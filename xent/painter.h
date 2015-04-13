#ifndef PAINTER_H
#define PAINTER_H

struct painter;

struct painter *
painter_alloc (
    struct ent_table * entities);

void
painter_free (
    struct painter * painter);

int
painter_paint (
    struct painter * painter,
    length_xy visual_size);

#endif//PAINTER_H
