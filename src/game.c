#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <string.h>
#include "game.h"

struct game *game_new(const size_t cell_size, const size_t rows, const size_t cols, const float frameSpeed)
{
    size_t w = cell_size*cols;
    size_t h = cell_size*rows;

    bool *d = calloc(cols*rows, sizeof(bool));
    if (!d) {
        perror("error allocating memory");
        return NULL;
    }

    bool *db = calloc(cols*rows, sizeof(bool));
    if (!db) {
        perror("error allocating memory");
        return NULL;
    }

    struct game *g = malloc(sizeof(struct game));
    if (!g) {
        perror("error allocating memory");
        return NULL;
    }
    g->screen_width = w;
    g->screen_height = h;
    g->rows = rows;
    g->cols = cols;
    g->cell_size = cell_size;
    g->frame_timer = GetTime();
    g->frame_timeout = frameSpeed;
    g->dish = d;
    g->dish_buf = db;
    g->is_running = true;
    g->pause = true;

    return g;
}

void game_update(struct game *g)
{
	if (IsKeyPressed(KEY_SPACE)) {
		g->pause = !g->pause;
	}

	if (IsKeyPressed(KEY_ENTER)) {
		game_restart(g);
	}

	if (IsKeyPressed(KEY_S)) {
		game_seed(g);
	}

	if (IsKeyPressed(KEY_Q)) {
		g->is_running = false;
	}

	// Add cell.
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		g->dish[get_mouse_over_cell(g)] = true;
	}
	// Kill cell.
	if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
		g->dish[get_mouse_over_cell(g)] = false;
	}

	// Control frame update.
	if (GetTime()-g->frame_timer < g->frame_timeout) {
		return;
	}
	g->frame_timer = GetTime();

	if (!g->pause) {
		memcpy(g->dish_buf, g->dish, g->rows*g->cols);

		for (size_t i = 0; i < g->rows*g->cols; i++) {
            g->dish_buf[i] = should_live(i, g);
		}

		memcpy(g->dish, g->dish_buf, g->rows*g->cols);
	}
}

void game_draw(struct game *g)
{
    BeginDrawing();
    ClearBackground(WHITE);

    size_t x, y;

	for (size_t i = 0; i < g->rows*g->cols; i++) {
		x = g->cell_size * (i % g->cols);
		y = g->cell_size * (i / g->cols);
		if (g->dish[i]) {
			DrawRectangle(x, y, g->cell_size, g->cell_size, BLACK);
			continue;
		}
		DrawRectangleLines(x, y, g->cell_size, g->cell_size, LIGHTGRAY);
	}

	game_draw_ui(g);

    EndDrawing();
}

void game_seed(struct game *g)
{
	// Random cell fill.
    srand(time(0));
	for (size_t i = 0; i < g->rows*g->cols; i++) {
		if (rand() % 7 == 0) {
			g->dish[i] = true;
		}
	}
}

void game_restart(struct game *g)
{
	g->pause = true;
	g->dish = memset(g->dish, false, g->rows*g->cols);
}

void game_draw_ui(struct game *g)
{
	Color bg_colour = WHITE;
	Color colour = DARKBLUE;
	if (g->pause) {
		bg_colour = DARKBLUE;
		colour = WHITE;
	}
	int padding = 5;
	int spacing = 8;
	int fontSize = 20;

	DrawRectangle(10, 10, 204, 28, bg_colour);
	int s1 = spacing + padding;
	DrawText("<space> - edit mode", 10+padding, s1, fontSize, colour);
	DrawText("<enter> - restart", 10+padding, s1*3, fontSize, DARKBLUE);
	DrawText("<q> - quit", 10+padding, s1*4+spacing, fontSize, DARKBLUE);

	if (g->pause) {
		DrawText("<s> - seed random cells", 10+padding, s1*6+spacing-2, fontSize, DARKBLUE);
		DrawText("<left-mouse> - add cell", 10+padding, s1*8+padding-3, fontSize, DARKBLUE);
		DrawText("<right-mouse> - kill cell", 10+padding, s1*10+padding-6, fontSize, DARKBLUE);

		size_t active_cell = get_mouse_over_cell(g);
		if (active_cell < 0 || active_cell >= g->rows*g->cols) {
			return;
		}

		char *live_text = "will die";
		Color live_colour = RED;
		bool live = should_live(active_cell, g);
		if (live) {
			live_text = "will live";
			live_colour = GREEN;
		}
		DrawText(live_text, GetMouseX()-5, GetMouseY()-10, fontSize-10, live_colour);
	}
}

bool should_live(const size_t i, const struct game *g)
{
	int left = 0, top_left = 0, bottom_left = 0;
    int right = 0, top_right = 0, bottom_right = 0;
    int top = 0, bottom = 0;

	if (i > 0) {
		left = g->dish[i-1];
	}
	if (i+1 < g->rows*g->cols) {
		right = g->dish[i+1];
	}

	if (i > g->cols && i-g->cols >= 0) {
		top = g->dish[i-g->cols];
	}
	if (i > g->cols && i-g->cols-1 >= 0) {
		top_left = g->dish[i-g->cols-1];
	}
	if (i > g->cols && i-g->cols+1 >= 0) {
		top_right = g->dish[i-g->cols+1];
	}

	if (i+g->cols < g->rows*g->cols) {
		bottom = g->dish[i+g->cols];
	}
	if (i+g->cols-1 < g->rows*g->cols) {
		bottom_left = g->dish[i+g->cols-1];
	}
	if (i+g->cols+1 < g->rows*g->cols) {
		bottom_right = g->dish[i+g->cols+1];
	}

	int neighbours = left + right + top + bottom + top_left + top_right + bottom_left + bottom_right;

	if (!g->dish[i]) {
		return neighbours == 3;
	}

	return neighbours >= 2 && neighbours <= 3;
}

size_t get_mouse_over_cell(struct game *g)
{
	size_t x = GetMouseX() / g->cell_size;
	size_t y = GetMouseY() / g->cell_size;
	return y*g->cols + x;
}

void game_free(struct game *g)
{
    free(g->dish);
    free(g->dish_buf);
    free(g);
}
