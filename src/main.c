/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
#include "pge.h"
  
#define MARGIN 3
#define WALL_WIDTH 8
#define PLAYER_WIDTH 10
static Window *s_game_window;

int currx, curry, boundx, boundy, default_left, default_right, default_bottom, curr_direction = 1, ball_moving = 0, difficulty_speed = 2;
int count = 0;

  
GPoint createPoint(int a, int b)
{
    GPoint usedPoint;
    usedPoint.x = a;
    usedPoint.y = b;
  return usedPoint;
};
 

static void move_ball () {
  //if (count == 0) {
    if (ball_moving == 1) {
      if (curr_direction == 1) {
        if (currx < default_right) {
          currx += difficulty_speed;
          if (currx > default_right) {
            currx = default_right;
          }
        }
        else {
          ball_moving = 0;
        }
      }
      else {
        
        if (currx != default_left) {
          currx -= difficulty_speed;
          if (currx < default_left) {
            currx = default_left;
          }
        }
        else {
          ball_moving = 0;
        }
      }
    //}
    //count = 0;
  }
  count++;
}


static void game_logic() {
  
  move_ball();

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
  switch (button_id) {
    case BUTTON_ID_UP:
      if (ball_moving == 0) {
        curr_direction = 1;
        ball_moving = 1;
      }
      
      break;
    case BUTTON_ID_DOWN:
      if (ball_moving == 0) {
        curr_direction = 0;
        ball_moving = 1;
      }
      break;
  }
}

/*static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
 
  curr_direction = 1;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {

  curr_direction = 0;
}


static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}*/

void pge_init() {
  // Start the game, keep a Window reference for later
  s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
  
  Layer *window_layer = window_get_root_layer(s_game_window);
  GRect bounds = layer_get_frame(window_layer);

  boundx = bounds.size.w;
  boundy = bounds.size.h;
  
  currx = MARGIN + WALL_WIDTH + PLAYER_WIDTH + MARGIN;
  curry = boundy - PLAYER_WIDTH - MARGIN;
  
  default_left = currx;
  default_right = boundx - (MARGIN + WALL_WIDTH + PLAYER_WIDTH + MARGIN); 
  default_bottom = curry;
  
  //window_set_click_config_provider(s_game_window, config_provider);
  
}


void pge_deinit() {
  // Finish the game
  pge_finish();
}