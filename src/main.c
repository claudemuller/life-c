#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#include "game.h"

int main(void)
{
    const int cell_size = 20;
    const int rows = 50;
    const int cols = 50;

    struct game *g = game_new(cell_size, rows, cols, 0.5);
    if (!g) {
        perror("error creating game struct");
        exit(1);
    }

    InitWindow(g->screen_width, g->screen_height, "Conway's Game of Life");

    SetTargetFPS(60);

    while (g->is_running && !WindowShouldClose()) {
        game_update(g);
        game_draw(g);
    }

    CloseWindow();

    game_free(g);

    return 0;
}
