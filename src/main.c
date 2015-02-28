/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
#include "pge.h"
  
#define MARGIN 3
#define WALL_WIDTH 8
#define PLAYER_WIDTH 10
static Window *s_game_window;

int currx, curry, boundx, boundy;

  
GPoint createPoint(int a, int b)
{
    GPoint usedPoint;
    usedPoint.x = a;
    usedPoint.y = b;
  return usedPoint;
};
 
static void game_logic() {
 
}

static void game_draw(GContext *ctx) {
  // Per-frame game rendering here
  
  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));
  graphics_context_set_fill_color(ctx, GColorFromRGB(255, 0, 0));

  graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  graphics_fill_rect(ctx, GRect(boundx - MARGIN - WALL_WIDTH, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  
  graphics_context_set_fill_color(ctx, GColorFromRGB(0, 255, 0));
  graphics_fill_circle(ctx, GPoint(currx, curry), PLAYER_WIDTH);
  
}

static void game_click(int button_id, bool long_click) {
  // Process click events
  
}

void pge_init() {
  // Start the game, keep a Window reference for later
  s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
  
  Layer *window_layer = window_get_root_layer(s_game_window);
  GRect bounds = layer_get_frame(window_layer);

  boundx = bounds.size.w;
  boundy = bounds.size.h;
  
  currx = MARGIN + WALL_WIDTH + PLAYER_WIDTH + MARGIN;
  curry = boundy - PLAYER_WIDTH - MARGIN;
  
}


void pge_deinit() {
  // Finish the game
  pge_finish();
}