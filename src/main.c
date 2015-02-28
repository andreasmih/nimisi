/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
 
#include "pge.h"

static Window *s_game_window;

int y = 0;

static void game_logic() {
  // Per-frame game logic here
  //if(x>1)
  //y ++;
  //x--;
}

static void game_draw(GContext *ctx) {
  // Per-frame game rendering here
  GPoint point, point2;
  point.x = 10;
  point.y = y + 10;
  point2.x = 40;
  point2.y = y + 40;
  
  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));
  graphics_draw_line(ctx, point,point2);
}

static void game_click(int button_id, bool long_click) {
  // Process click events
  
}

void pge_init() {
  // Start the game, keep a Window reference for later
  s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
}


void pge_deinit() {
  // Finish the game
  pge_finish();
}