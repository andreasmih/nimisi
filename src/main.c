/**
 * This is the bare minimum to make a looping game with PGE!
 */

#include "main.h"
  
#define MARGIN 3
#define WALL_WIDTH 8
#define BALL_RADIUS 10
#define HOLE_WIDTH (BALL_RADIUS * 4)
#define MAX_LIMIT -200
#define MIN_LIMIT -100
#define BALL_UPDATE_FREQ 0
#define HOLES_UPDATE_FREQ 0
  
static Window *s_game_window;
static TextLayer *s_text_layer, *s_score_layer;

static char s_score_buffer[32];
static int s_score = 0;

int ball_radius = BALL_RADIUS;
int currx, curry;
int boundx, boundy;
int default_left, default_right, default_bottom; 
int curr_direction = 0, ball_moving = 0;
int ball_count = 0, holes_count = 0;
int difficulty_speed = 10, hole_speed = 2;
bool lost_game = 0, show_final_score = 0;

  
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


int themes[10][10][3] = {{{177,235,0},{83,187,244},{255,133,203},{255,67,46},{255,172,0}}, {{241,125,128},{115,116,149},{104,168,173},{196,212,175},{108,134,114}}, {{11,153,188},{92,45,80},{212,14,82},{205,23,25},{252,224,20}}};
int themes_no = 3;
int random_colors = 0;
int single_theme_index;
int theme_index;

int get_random_theme () {
	int random = rand() % themes_no;
	return random;
}



static void update_status_text() {
  snprintf(s_score_buffer, sizeof(s_score_buffer), "Score: %d", s_score);
  text_layer_set_text(s_score_layer, s_score_buffer);
}

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
          if (default_bottom - hole_list[i].y < HOLE_WIDTH / 1.1 && default_bottom - hole_list[i].y > HOLE_WIDTH - HOLE_WIDTH / 1.1) {
            lost_game = 1;
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

	if (curry > default_bottom - 2) {
		difficulty_speed+=1;
		curry--;
	}

	if (curr_direction == 1 && currx < boundx + ball_radius) {
		currx+=difficulty_speed;
	}
	else {
		if (curr_direction == 0 && currx > 0 - ball_radius - 1) {
	      currx-=difficulty_speed;
	    }
	    else {
	    	show_final_score = 1;
	    	text_layer_destroy(s_score_layer);
	    }
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
  if (holes_count == HOLES_UPDATE_FREQ) {
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
          hole_list[i].y += hole_speed;
        }
      }
    }
    holes_count = 0;
  }
  if (HOLES_UPDATE_FREQ != 0) {
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
  
  
  if (!lost_game) {
    move_ball();
    move_holes();

    s_score++;
    update_status_text();

    theme_index = single_theme_index;
  	if (random_colors == 1) {
  		theme_index = get_random_theme();
  	}

  }
  else {
  	if (show_final_score != 1) {
    	explode_ball();
  	}
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
  
  if (show_final_score == 0) {
  	  

	  graphics_context_set_stroke_color(ctx, GColorFromRGB(255, 0, 0));

	  

	  graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][0][0], themes[theme_index][0][1], themes[theme_index][0][2]));

	  graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
	  graphics_fill_rect(ctx, GRect(boundx - MARGIN - WALL_WIDTH, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
	  

	  


	  graphics_context_set_fill_color(ctx, GColorBlack);
	  draw_holes (ctx);
	  
	  graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][1][0], themes[theme_index][1][1], themes[theme_index][1][2]));

	  graphics_fill_circle(ctx, GPoint(currx, curry), ball_radius);


	  graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][2][0], themes[theme_index][2][1], themes[theme_index][2][2]));

	  graphics_fill_rect(ctx, GRect(0, 0, MARGIN, boundy), 0, 0);
	  graphics_fill_rect(ctx, GRect(0, 0, boundx, MARGIN), 0, 0);
	  graphics_fill_rect(ctx, GRect(boundx - MARGIN, MARGIN, MARGIN, boundy - MARGIN), 0, 0);
	  graphics_fill_rect(ctx, GRect(MARGIN, boundy - MARGIN, boundx- 2*MARGIN, MARGIN), 0, 0);

}
else {

	  graphics_draw_text(ctx, "GAME OVER!", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(10, boundy/2 - 30 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, "FINAL SCORE:", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(10, boundy/2 - 10 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
      graphics_draw_text(ctx, s_score_buffer + 7, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(10, boundy/2 + 10 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
 }

  //graphics_fill_rect(ctx, GRect(MARGIN, MARGIN + hole_pos, WALL_WIDTH, ball_radius * 2), 0, 0);
  
}

static void game_click(int button_id, bool long_click) {
  switch (button_id) {
    case BUTTON_ID_UP:
      if (ball_moving == 0 && lost_game != 1) {
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
      if (ball_moving == 0 && lost_game != 1) {
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
 	  
 	  if (random_colors == 0)
      random_colors = 1;
  		else
  			random_colors = 0;

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

static void splash_done_handler() {
  // Create player's Ship


  single_theme_index = get_random_theme();
  // Begin game loop
  s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
  pge_set_framerate(30);

  // Create a window_layer to get screen boundaries
  
  Layer *window_layer = window_get_root_layer(s_game_window);
  GRect bounds = layer_get_frame(window_layer);

  boundx = bounds.size.w;
  boundy = bounds.size.h;
  
  // Set the initial position of the ball

  currx = MARGIN + WALL_WIDTH + ball_radius + MARGIN;
  curry = boundy - ball_radius - MARGIN;
  
  default_left = currx;
  default_right = boundx - (MARGIN * 2 + WALL_WIDTH + ball_radius); 
  default_bottom = curry;

  
  
  // Initialize the holes in the walls

  init_holes();

  // Create score layer

  s_score_layer = text_layer_create(GRect(25, 0, boundx - 60, 20));
  text_layer_set_text_color(s_score_layer, GColorWhite);
  text_layer_set_background_color(s_score_layer, GColorBlack);
  text_layer_set_font(s_score_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_get_root_layer(s_game_window), text_layer_get_layer(s_score_layer));  

  // Setup score
  update_status_text();

  // Begin spawn loop
  //app_timer_register(rand() % MAX_SPAWN_INTERVAL, spawn_handler, NULL);
}

void pge_init() {
  srand(time(NULL));
  
  
 //srand(time(NULL));

  pge_splash_show(splash_done_handler);
  
}




void pge_deinit() {
	// Destroy score layer
	if (show_final_score != 1) {
		text_layer_destroy(s_score_layer);
	}
	// Finish game
	pge_finish();
}
