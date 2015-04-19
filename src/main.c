/*
 * main.c
 * Creates a Window, Layer and assigns an `update_proc` to draw 
 * the 'P' in the Pebble logo.
 */

#include <pebble.h>
#include <math.h>
#include <stdlib.h>

static Window *s_main_window;
static Layer *s_pad_layer;
static TextLayer *scr;
static int pos=60;
static GPoint posb;
static GPoint speed;
static int bounces;
static int score;


char *itoa(int num)
{
static char buff[20] = {};
int i = 0, temp_num = num, length = 0;
char *string = buff;
if(num >= 0) {
// count how many characters in the number
while(temp_num) {
temp_num /= 10;
length++;
}
// assign the number to the buffer starting at the end of the 
// number and going to the begining since we are doing the
// integer to character conversion on the last number in the
// sequence
for(i = 0; i < length; i++) {
buff[(length-1)-i] = '0' + (num % 10);
num /= 10;
}
buff[i] = '\0'; // can't forget the null byte to properly end our string
}
else
return "Unsupported Number";
return string;
}



static void pad_update_proc(Layer *this_layer, GContext *ctx) {

 	//Set bg to black
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(s_pad_layer), 0, GCornerNone);
	
	//bounds for paddle
	if(pos<0){
		pos =0;
	}
	if(pos>105){
		pos = 105;
	}
	//Draw paddle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(10, pos , 7, 45), 0, GCornerNone);
	
	//direction and speed of ball
	if(posb.x>=130){
		speed.x = speed.x * -1;
	}
	if(posb.y<=5 || posb.y>=140){
		speed.y = speed.y * -1;
		bounces += 1;
	}
	if(posb.x<=20){
		if(abs(posb.y+5-pos-22)<23){
			if(abs(posb.y-pos-11)<12){
				if(speed.y<7){speed.y+=1;}
				else{speed.y=7;}
			}
			else if(abs(posb.y-pos+11)<12){
				if(speed.y>-7){speed.y-=1;}
				else{speed.y=-7;}
			}
			else{speed.x*=-1;}
			
			posb.x += 5;
		}
		
		else{
			posb = GPoint(72, 84);
	
			speed.y = (rand()%5)*((2*rand()%2)-1);
			speed.x = (rand()%11)*((2*rand()%2)-1);
			score-=1;
		}
	}
	
	//pos for ball
	posb.x += speed.x;
	posb.y += speed.y;
	
	graphics_fill_rect(ctx, GRect(posb.x, posb.y, 10, 10), 0, GCornerNone);
	
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	pos -= 10;
	layer_mark_dirty(s_pad_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(s_output_layer, "Select pressed!");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	pos += 10;
	layer_mark_dirty(s_pad_layer);
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 90, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 90, down_click_handler);
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
	
	scr = text_layer_create(GRect(0, 30, 144, 50));
  text_layer_set_background_color(scr, GColorClear);
  text_layer_set_text_color(scr, GColorWhite);
	
	char *scor= itoa(score);
  text_layer_set_text(scr, scor);

  // Create Layer
  s_pad_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, s_pad_layer);
	layer_add_child(window_layer, text_layer_get_layer(scr));

  // Set the update_proc
  layer_set_update_proc(s_pad_layer, pad_update_proc);
	
}

static void main_window_unload(Window *window) {
  // Destroy Layer
  layer_destroy(s_pad_layer);
	text_layer_destroy(scr);
}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_stack_push(s_main_window, true);
	
	posb = GPoint(72, 84);
	
	speed.y = (rand()%5)*((2*rand()%2)-1);
	speed.x = (rand()%11)*((2*rand()%2)-1);
	
	bounces = 0;
	score = 7;
	
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
