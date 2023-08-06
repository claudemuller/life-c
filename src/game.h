#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>

struct game {
	int screen_width;
	int screen_height;
    size_t rows;
    size_t cols;
    size_t cell_size;
    float frame_timer;
    float frame_timeout;
	bool *dish;
	bool *dish_buf;
	bool pause;
	bool is_running;
};

struct game *game_new(const size_t cell_size, const size_t rows, const size_t cols, const float frameSpeed);
void game_update(struct game *g);
void game_draw(struct game *g);
void game_restart(struct game *g);
void game_seed(struct game *g);
void game_draw_ui(struct game *g);
bool should_live(const size_t i, const struct game *g);
size_t get_mouse_over_cell(struct game *g);
void game_free(struct game *g);

#endif // GAME_H
