#ifndef XENT_H
#define XENT_H

typedef float length_xy[2]; // meters

enum input_type
{
	input_type_touch = 1,
	input_type_mouse,
	input_type_key,
};

typedef int input_id;

enum input_action
{
	input_action_begin = 1,
	input_action_update,
	input_action_end,
};

#endif//XENT_H
