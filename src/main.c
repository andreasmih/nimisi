/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
#include "pge.h"
  
#define MARGIN 3
#define WALL_WIDTH 8
#define PLAYER_WIDTH 10
#define MAX_LIMIT -200
#define MIN_LIMIT -100
  
static Window *s_game_window;

int currx, curry, boundx, boundy, default_left, default_right, default_bottom, curr_direction = 1, ball_moving = 0, difficulty_speed = 2, hole_pos = 0;
int count = 0;

  
GPoint createPoint(int a, int b)
{
    GPoint usedPoint;
    usedPoint.x = a;
    usedPoint.y = b;
  return usedPoint;
};
 

static void move_ball () {
  if (count == 3) {
    hole_pos ++;
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
    }
    count = 0;
  }
  count++;
}

struct wall_hole {
   int wall_side;
   int y;
} hole_list[10];



/*void get_random_hole (struct wall_hole hole) {

  int wall = 999, y = 0;
  
  if(rand() % 2 == 1) {
    wall = 1;
  }
  else {
    wall = 0;
  }
  
  if(rand() % 2 == 1) {
    wall = 999;    
  }
  else {
     y = - ((rand() % (-MAX_LIMIT+MIN_LIMIT)) - MIN_LIMIT);
  }
  
  hole.wall_side = wall;
  hole.y = y;

}*/

int get_random_wall (int wall) {
  
  if(rand() % 2 == 1) {
    wall = 1;
  }
  else {
    wall = 0;
  }
  
  if(rand() % 2 == 1) {
    wall = 999;    
  }
  
  return wall;

}

int get_random_y (int y) {
  
  
  if(rand() % 5 == 1) {
       y = - ((rand() % (-MAX_LIMIT+MIN_LIMIT)) - MIN_LIMIT);
  }

  return y;

}

static void game_logic() {
  
  move_ball();
  int i = 0;
  for (i = 0; i < 6; i++) {
    
    if (hole_list[i].wall_side == 999) {
      hole_list[i].wall_side = get_random_wall(hole_list[i].wall_side);
      hole_list[i].y = get_random_y(hole_list[i].y);
    }
    else {
    
      if (hole_list[i].y > boundy) {
        hole_list[i].wall_side = 999;
      }
      else {
        hole_list[i].y ++;
      }
    }
    
    
  }
  
  
  
}

static void draw_holes(GContext *ctx) {
  int length = hole_list[9].wall_side;
  int i = 0;
  for (i = 0; i < length; i++) {
    if (hole_list[i].wall_side != 999) {
      graphics_fill_rect(ctx, GRect(MARGIN + hole_list[i].wall_side * (boundx - 2 * MARGIN - WALL_WIDTH), MARGIN + hole_list[i].y, WALL_WIDTH, PLAYER_WIDTH * 2), 0, 0);
    }
  }
}

static void game_draw(GContext *ctx) {
  // Per-frame game rendering here
  
  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));
  graphics_context_set_fill_color(ctx, GColorFromRGB(255, 0, 0));

  graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  graphics_fill_rect(ctx, GRect(boundx - MARGIN - WALL_WIDTH, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  
  graphics_context_set_fill_color(ctx, GColorFromRGB(0, 255, 0));
  graphics_fill_circle(ctx, GPoint(currx, curry), PLAYER_WIDTH);
  
  graphics_context_set_fill_color(ctx, GColorFromRGB(0, 0, 255));
  draw_holes (ctx);
  //graphics_fill_rect(ctx, GRect(MARGIN, MARGIN + hole_pos, WALL_WIDTH, PLAYER_WIDTH * 2), 0, 0);
  
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
    
    case BUTTON_ID_SELECT:
      hole_pos =0;
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
  default_right = boundx - (MARGIN * 2 + WALL_WIDTH + PLAYER_WIDTH); 
  default_bottom = curry;
  
  hole_list[0].y = 0;
  hole_list[1].y = 30;
  hole_list[2].y = 60;
  hole_list[3].y = 0;
  hole_list[3].wall_side = 1;
  hole_list[4].y = 30;
  hole_list[4].wall_side = 1;
  hole_list[5].y = 90;
  hole_list[5].wall_side = 1;
  hole_list[9].wall_side = 6;
  //window_set_click_config_provider(s_game_window, config_provider);
  
}




void pge_deinit() {
  // Finish the game
  pge_finish();
}