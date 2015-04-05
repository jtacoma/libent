#ifndef ENTLIB_MODEL_H
#define ENTLIB_MODEL_H

bool
ent_model_has (
    struct ent_model * m,
    char const * table_name);

struct ent_table *
ent_model_get_const (
    struct ent_model * m,
    char const * table_name);

#endif//ENTLIB_MODEL_H
