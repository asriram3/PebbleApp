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
static int pos;
static int sp;
static GPoint posb;
static GPoint speed;
static int bounces;
static int score;






static void pad_update_proc(Layer *this_layer, GContext *ctx) {

 	//Set bg to black
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(s_pad_layer), 0, GCornerNone);
	
	//bounds for paddle
	pos += sp;
	if(pos<=0){
		pos =0;
	}
	else if(pos>=105){
		pos = 105;
	}
	else{
		//pos += sp;
	}
	//Draw paddle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(10, pos , 7, 45), 0, GCornerNone);
	
	//direction and speed of ball
	if(posb.x>=130){
		speed.x = speed.x * -1;
		score+=1;
	}
	if(posb.y<=5 || posb.y>=140){
		speed.y = speed.y * -1;
		bounces += 1;
	}
	if(posb.x>=16 && posb.x<20){
		if(abs(posb.y+5-(pos+23))<23){
			if(abs(posb.y-pos-11)<12){
				if(speed.y<7){speed.y+=1;}
				else{speed.y=7;}
			}
			else if(abs(posb.y-pos+11)<12){
				if(speed.y>-7){speed.y-=1;}
				else{speed.y=-7;}
			}
			speed.x*=-1;
			
			//posb.x += 5;
		}
		
		if(posb.x<=5){
			posb = GPoint(72, 84);
	
			//speed.y = (rand()%1)*((2*rand()%2)-1);
			//speed.x = (rand()%2)*((2*rand()%2)-1);
			score-=1;
		}
	}
	
	//pos for ball
	posb.x += speed.x;
	posb.y += speed.y;
	
	graphics_fill_rect(ctx, GRect(posb.x, posb.y, 10, 10), 0, GCornerNone);
	
}

static void data_handler(AccelData *data, uint32_t num_samples) {
	
	if(data[0].y<-75){sp = 3;}
	else if(data[0].y>75){sp=-3;}
	else{sp=0;}
	layer_mark_dirty(s_pad_layer);
	
}


static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	//pos -= 10;
	//layer_mark_dirty(s_pad_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(s_output_layer, "Select pressed!");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	//pos += 10;
	//layer_mark_dirty(s_pad_layer);
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
	
	scr = text_layer_create(GRect(50, 0, 144, 50));
  text_layer_set_background_color(scr, GColorClear);
  text_layer_set_text_color(scr, GColorWhite);
	
	static char scor[32];
	snprintf(scor, sizeof(scor), "Score: %i", score);
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
	
	int num_samples = 3;
  accel_data_service_subscribe(num_samples, data_handler);
	accel_service_set_sampling_rate(ACCEL_SAMPLING_100HZ);
	
	pos=60;
	sp = 0;
	posb = GPoint(72, 84);
	
	speed.y = (rand()%1)*((2*rand()%2)-1);
	speed.x = (rand()%2)*((2*rand()%2)-1);
	
	bounces = 0;
	score = 7;
	
}

static void deinit(void) {
  // Destroy main Window
  window_destroy(s_main_window);
	accel_data_service_unsubscribe();
	
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
