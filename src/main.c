#include "main.h"
  

#define HIGHSCORE_KEY 1

#define MARGIN 3
#define WALL_WIDTH 8
#define BALL_RADIUS 10
#define HOLE_WIDTH (BALL_RADIUS * 4)
#define NEW_HOLE_MIN_Y -200
#define NEW_HOLE_MAX_Y -100
#define BALL_UPDATE_FREQ 0
#define HOLES_UPDATE_FREQ 0
  
static Window *s_game_window;
static TextLayer *s_text_layer, *s_score_layer;

static char s_score_buffer[32];
static char s_highscore_buffer[32];
static int s_score = 0;
static int s_highscore = 0;

int ball_radius = BALL_RADIUS;
int currx, curry;
int boundx, boundy;
int default_left, default_right, default_bottom; 
int curr_direction = 0, ball_moving = 0;
int ball_count = 0, holes_count = 0;
int ball_speed = 10, hole_speed = 2;
bool lost_game = 0, show_final_score = 0;

int themes[][5][3] = {{{177,235,0},{83,187,244},{255,133,203},{255,67,46},{255,172,0}}, {{241,125,128},{115,116,149},{104,168,173},{196,212,175},{108,134,114}}, {{11,153,188},{92,45,80},{212,14,82},{205,23,25},{252,224,20}}};
int total_themes = 3;
int random_colors = 0;
int single_theme_index;
int theme_index;

struct wall_hole {
 	int wall_side;
 	int y;
} hole_list[10];
int total_holes = 6;

int get_random_theme() {

	return rand() % total_themes;
}

static void update_status_text() {

	snprintf(s_score_buffer, sizeof(s_score_buffer), "Score: %d", s_score);
}

static void update_highscore() {

	snprintf(s_highscore_buffer, sizeof(s_highscore_buffer), "%d", s_highscore);
}

static void move_ball() {

	if (ball_count == BALL_UPDATE_FREQ) {
		if (ball_moving == 1) {
			if (curr_direction == 1) {
				if (currx < default_right) {
					currx += ball_speed;
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
			  		currx -= ball_speed;
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
			for (i = 0; i < total_holes; i++) {
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

/*void get_random_hole(struct wall_hole hole) {

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
     y = - ((rand() % (-NEW_HOLE_MIN_Y+NEW_HOLE_MAX_Y)) - NEW_HOLE_MAX_Y);
  }
  
  hole.wall_side = wall;
  hole.y = y;

}*/

int get_hole_random_wall(int wall) {

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

int get_hole_random_y(int y) {

	y = - ((rand() % (-NEW_HOLE_MIN_Y+NEW_HOLE_MAX_Y)) - NEW_HOLE_MAX_Y);
  	return y;
}

int myabs(int a,int b){

	if (a > b){
		return a - b;
	}
	else { 
		return b-a;
	}
}

static void move_holes() {

	int i = 0;
	if (holes_count == HOLES_UPDATE_FREQ) {
		for (i = 0; i < total_holes; i++) { 
			if (hole_list[i].wall_side == 999) {
				hole_list[i].wall_side = get_hole_random_wall(hole_list[i].wall_side);
				if (hole_list[i].wall_side != 999) {
					int a = get_hole_random_y(hole_list[i].y);
					int j = 0;
					bool ok = 1;
					for (j = 0; j < total_holes; j++) {
						if (i!=j) {
							if (myabs(a, hole_list[j].y) < HOLE_WIDTH + 10) {
					    		ok = 0;  
					  		}
						}
					}
					if(ok) {
						hole_list[i].y = a;
					}
					else {
						hole_list[i].wall_side = 999;
					}
		    	}
      		}
			else {
				hole_list[i].y += hole_speed;
				if (hole_list[i].y > boundy) {
					hole_list[i].wall_side = 999;
				}
			}
		}
		holes_count = 0;
	}
	if (HOLES_UPDATE_FREQ != 0) {
		holes_count++;
	}
}

static void explode_ball() {

	if (curry > default_bottom - 2) {
		ball_speed+=1;
		curry--;
	}
	if (curr_direction == 1 && currx < boundx + ball_radius) {
		currx+=ball_speed;
	}
	else {
		if (curr_direction == 0 && currx > 0 - ball_radius - 1) {
			currx -= ball_speed;
	    }
	    else {
	    	if (persist_exists(HIGHSCORE_KEY)) {
				s_highscore = persist_read_int(HIGHSCORE_KEY);
				if (s_score > s_highscore) {
					persist_write_int(HIGHSCORE_KEY, s_score);
					s_highscore = s_score;
				}
			}
			else {
				persist_write_int(HIGHSCORE_KEY, s_score);
				s_highscore = s_score;
			}
			update_highscore();
	    	show_final_score = 1;
	    }
	}
}

static void game_logic() {

  	if (!lost_game) {
		move_ball();
		move_holes();

		s_score++;
		update_status_text();

		hole_speed = (s_score / 1500) + 2;
		ball_speed = (s_score / 1500) + 10;

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

static void draw_walls(GContext *ctx) {

	graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][0][0], themes[theme_index][0][1], themes[theme_index][0][2]));

	graphics_fill_rect(ctx, GRect(MARGIN, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
	graphics_fill_rect(ctx, GRect(boundx - MARGIN - WALL_WIDTH, MARGIN, WALL_WIDTH, boundy - 2 * MARGIN), 0, 0);
}

static void draw_holes(GContext *ctx) {

	graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][4][0], themes[theme_index][4][1], themes[theme_index][4][2]));

	int i = 0;
	for (i = 0; i < total_holes; i++) {
	    if (hole_list[i].wall_side != 999 && hole_list[i].y > -HOLE_WIDTH) {
			graphics_fill_rect(ctx, GRect(MARGIN + hole_list[i].wall_side * (boundx - 2 * MARGIN - WALL_WIDTH), MARGIN + hole_list[i].y, WALL_WIDTH, HOLE_WIDTH), 0, 0);
	    }
	}
}

static void draw_score(GContext *ctx) {

	graphics_context_set_text_color(ctx, GColorFromRGB(themes[theme_index][1][0], themes[theme_index][1][1], themes[theme_index][1][2]));

	graphics_draw_text(ctx, s_score_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(13, 0, boundx - 26, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void draw_ball(GContext *ctx) {
	graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][1][0], themes[theme_index][1][1], themes[theme_index][1][2]));
	graphics_fill_circle(ctx, GPoint(currx, curry), ball_radius);
}

static void draw_margins(GContext *ctx) {

	graphics_context_set_fill_color(ctx, GColorFromRGB(themes[theme_index][3][0], themes[theme_index][3][1], themes[theme_index][3][2]));

	graphics_fill_rect(ctx, GRect(0, 0, MARGIN, boundy), 0, 0);
	graphics_fill_rect(ctx, GRect(0, 0, boundx, MARGIN), 0, 0);
	graphics_fill_rect(ctx, GRect(boundx - MARGIN, MARGIN, MARGIN, boundy - MARGIN), 0, 0);
	graphics_fill_rect(ctx, GRect(MARGIN, boundy - MARGIN, boundx- 2*MARGIN, MARGIN), 0, 0);
}

static void draw_final_score(GContext *ctx) {

	graphics_context_set_text_color(ctx, GColorFromRGB(themes[theme_index][1][0], themes[theme_index][1][1], themes[theme_index][1][2]));

	graphics_draw_text(ctx, "GAME OVER!", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(10, 20 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
	graphics_draw_text(ctx, "FINAL SCORE:", fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(10, 40 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
	graphics_draw_text(ctx, s_score_buffer + 7, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(10, 60 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void draw_highscore(GContext *ctx) {

	graphics_context_set_text_color(ctx, GColorFromRGB(themes[theme_index][1][0], themes[theme_index][1][1], themes[theme_index][1][2]));

	graphics_draw_text(ctx, s_highscore_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), GRect(10, 110 , boundx - 20, 20), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void game_draw(GContext *ctx) {

	if (show_final_score == 0) {
  	  
		window_set_background_color (s_game_window, GColorFromRGB(themes[theme_index][4][0], themes[theme_index][4][1], themes[theme_index][4][2]));

		draw_walls(ctx);
		draw_holes (ctx);
		draw_score(ctx);
		draw_ball(ctx);
		draw_margins(ctx);
	}
	else {
		draw_final_score(ctx);
		draw_highscore(ctx);
	}
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
				s_score += 50;
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
				s_score += 50;
			}
			break;

		case BUTTON_ID_SELECT:
			if (random_colors == 0) {
				random_colors = 1;
			}
			else {
				random_colors = 0;
			}
			break;
	}
}

void init_ball() {

	default_left = MARGIN + WALL_WIDTH + ball_radius + MARGIN;
	default_right = boundx - (MARGIN * 2 + WALL_WIDTH + ball_radius + 1); 
	default_bottom = boundy - ball_radius - MARGIN - 1;
	currx = default_left;
	curry = default_bottom;
} 

void init_holes() {

	int i = 0;
	for (i = 0; i < total_holes; i++) {
		hole_list[i].y = -150;
	}
	hole_list[0].wall_side = 0;
}

static void splash_done_handler() {

	// Begin game loop
	s_game_window = pge_begin(GColorBlack, game_logic, game_draw, game_click);
	pge_set_framerate(30);

	// Create a window_layer to get screen boundaries
	Layer *window_layer = window_get_root_layer(s_game_window);
	GRect bounds = layer_get_frame(window_layer);
	boundx = bounds.size.w;
	boundy = bounds.size.h;

	//Get a random theme
	single_theme_index = get_random_theme();

	// Set the initial position of the ball
	init_ball();

	// Initialize the holes in the walls
	init_holes();

	// Setup score
	update_status_text();
}

void pge_init() {

	srand(time(NULL));
	pge_splash_show(splash_done_handler);
}

void pge_deinit() {

	// Finish game
	pge_finish();
}
