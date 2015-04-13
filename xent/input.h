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
input_append (
    struct input * input,
    enum input_type type,
    input_id id,
    enum input_action action,
    length_xy pos);

#endif//INPUT_H
