#ifndef GAME_H
#define GAME_H

struct game;

struct game *
game_alloc (
    struct ent_table * entities);

void
game_free (
    struct game * game);

int
game_logic (
    struct game * game);

#endif//GAME_H
