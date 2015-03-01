/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include <pebble.h>
#include "pge.h"
  
#define MARGIN 3
#define WALL_WIDTH 8
#define PLAYER_WIDTH 10
#define HOLE_WIDTH (PLAYER_WIDTH * 4)
#define MAX_LIMIT -200
#define MIN_LIMIT -100
#define BALL_UPDATE_FREQ 0
#define WALL_UPDATE_FREQ 3
  
static Window *s_game_window;

int player_width = PLAYER_WIDTH;
int currx, curry, boundx, boundy, default_left, default_right, default_bottom, curr_direction = 0, ball_moving = 0, difficulty_speed = 2;
int ball_count = 0, holes_count = 0;
bool close_program = 0;

  
GPoint createPoint(int a, int b)
{
    GPoint usedPoint;
    usedPoint.x = a;
    usedPoint.y = b;
  return usedPoint;
};
 
struct wall_hole {
   int wall_side;
   int y;
} hole_list[10];



int get_random_wall (int wall) {
  
  if(rand() % 2 == 1) {
    wall = 1;
  }
  else {
    wall = 0;
  }
  
  if(rand() % 15 == 1) {
    wall = 999;    
  }
  
  return wall;

}

int get_random_y (int y) {
  

  y = - ((rand() % (-MAX_LIMIT+MIN_LIMIT)) - MIN_LIMIT);
  

  return y;

}


static void move_ball () {
  if (ball_count == BALL_UPDATE_FREQ) {
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
    else {
      int i = 0;
      for (i = 0; i < 6; i++) {
        if (curr_direction == hole_list[i].wall_side ) {
          if (default_bottom - hole_list[i].y < HOLE_WIDTH && default_bottom - hole_list[i].y > 0) {
            close_program = 1;
          }  
        }
      }
    }
    ball_count = 0;
  }
  if (BALL_UPDATE_FREQ != 0) {
    ball_count++;
  }
  
}

static void explode_ball () {
  
  if (curry > boundy/2) {
    curry--;
  }
  
  if (curr_direction == 1) {
    if (currx > boundx/2) {
      currx--;
    }
  }
  else {
    if (currx < boundx/2) {
      currx++;
    }
  }
  player_width++;
  if (player_width > boundy + 20) {
    
  }
  
}

int myabs(int a,int b){
  if (a > b){
    return a - b;
  }
  else return b-a;
  
}

static void move_holes () {
  int i = 0;
  if (holes_count == WALL_UPDATE_FREQ) {
    for (i = 0; i < 6; i++) { 
      if (hole_list[i].wall_side == 999) {
        hole_list[i].wall_side = get_random_wall(hole_list[i].wall_side);
        if (hole_list[i].wall_side != 999) {
          int a = get_random_y(hole_list[i].y);
          int j = 0;
          bool ok = 1;
          for (j = 0; j < 6; j++) {
            if (i!=j) {
              if (myabs(a, hole_list[j].y) < HOLE_WIDTH + 10) {
                ok = 0;  
              }
              
            }
          }
          if(ok) {
            hole_list[i].y = a;
          }
        }
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
    holes_count = 0;
  }
  if (WALL_UPDATE_FREQ != 0) {
    holes_count++;
  }

}





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



static void game_logic() {
  
  if (!close_program) {
    move_ball();
  
    move_holes();
  }
  
  else {
    explode_ball();
  }
  
}

static void draw_holes(GContext *ctx) {
  int length = hole_list[9].wall_side;
  int i = 0;
  for (i = 0; i < length; i++) {
    if (hole_list[i].wall_side != 999) {
      graphics_fill_rect(ctx, GRect(MARGIN + hole_list[i].wall_side * (boundx - 2 * MARGIN - WALL_WIDTH), MARGIN + hole_list[i].y, WALL_WIDTH, HOLE_WIDTH), 0, 0);
    }
  }
}

static void game_draw(GContext *ctx) {
  // Per-frame game rendering here
  
  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));
  graphics_context_set_fill_color(ctx, GColorFromRGB(255, 0, 0));

  graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  graphics_fill_rect(ctx, GRect(boundx - MARGIN - WALL_WIDTH, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  draw_holes (ctx);
  
  graphics_context_set_fill_color(ctx, GColorFromRGB(0, 255, 0));
    
  graphics_fill_circle(ctx, GPoint(currx, curry), player_width);

  //graphics_fill_rect(ctx, GRect(MARGIN, MARGIN + hole_pos, WALL_WIDTH, player_width * 2), 0, 0);
  
}

static void game_click(int button_id, bool long_click) {
  switch (button_id) {
    case BUTTON_ID_UP:
      if (ball_moving == 0) {
        if (curr_direction == 0) {
          curr_direction = 1;
        }
        else {
          curr_direction = 0;
        }
        ball_moving = 1;
      }
      
      break;
    case BUTTON_ID_DOWN:
      if (ball_moving == 0) {
        if (curr_direction == 0) {
          curr_direction = 1;
        }
        else {
          curr_direction = 0;
        }
        ball_moving = 1;
      }
      break;
    
    case BUTTON_ID_SELECT:
 
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


void init_holes () {
  int i = 0;
  hole_list[9].wall_side = 6;
  for (i = 0; i < hole_list[9].wall_side; i++) {
    hole_list[i].y = -150;
    
  }

 
}

void pge_init() {
  // Start the game, keep a Window reference for later
  s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
  
  Layer *window_layer = window_get_root_layer(s_game_window);
  GRect bounds = layer_get_frame(window_layer);

  boundx = bounds.size.w;
  boundy = bounds.size.h;
  
  currx = MARGIN + WALL_WIDTH + player_width + MARGIN;
  curry = boundy - player_width - MARGIN;
  
  default_left = currx;
  default_right = boundx - (MARGIN * 2 + WALL_WIDTH + player_width); 
  default_bottom = curry;
  

  init_holes();
  //window_set_click_config_provider(s_game_window, config_provider);
  
}




void pge_deinit() {
  // Finish the game
  pge_finish();
}