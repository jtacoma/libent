#ifndef INPUT_H
#define INPUT_H

struct input;

struct input *
input_alloc (
    struct ent_table * entities);

void
input_free (
    struct input * input);

int
input_begin (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos);

int
input_update (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos);

int
input_end (
    struct input * input,
    enum input_type type,
    input_id id,
    length_xy pos);

#endif//INPUT_H
