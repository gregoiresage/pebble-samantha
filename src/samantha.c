#include "pebble.h"

static Window *window;
static Layer *minutes_layer;
static Layer *seconds_layer;

static uint8_t hours    = 0;
static uint8_t minutes  = 0;
static uint8_t seconds  = 0;
static bool bluetooth_connected = false;

static GPath *hour_path;
static const GPathInfo HOUR_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {14, 14},
    {-14, 14},
  }
};

void minutes_layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);

  GPoint p1 = GPoint(53, 62);
  GPoint p2 = GPoint(89, 62);
  GPoint p3 = GPoint(53, 98);
  GPoint p4 = GPoint(89, 98);

  switch(hours % 12){
    case 0 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 270 / 360);
      gpath_move_to(hour_path, GPoint(p4.x+1,p4.y));
      gpath_draw_filled(ctx, hour_path);
    case 11 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 180 / 360);
      gpath_move_to(hour_path, GPoint(p4.x,p4.y-1));
      gpath_draw_filled(ctx, hour_path);
    case 10 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 90 / 360);
      gpath_move_to(hour_path, GPoint(p4.x-1,p4.y));
      gpath_draw_filled(ctx, hour_path);
    case  9 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 270 / 360);
      gpath_move_to(hour_path, GPoint(p3.x+1,p3.y));
      gpath_draw_filled(ctx, hour_path);
    case  8 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 180 / 360);
      gpath_move_to(hour_path, GPoint(p3.x,p3.y-1));
      gpath_draw_filled(ctx, hour_path);
    case  7 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 90 / 360);
      gpath_move_to(hour_path, GPoint(p3.x-1,p3.y));
      gpath_draw_filled(ctx, hour_path);
    case  6 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 270 / 360);
      gpath_move_to(hour_path, GPoint(p2.x+1,p2.y));
      gpath_draw_filled(ctx, hour_path);
    case  5 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 0 / 360);
      gpath_move_to(hour_path, GPoint(p2.x,p2.y+1));
      gpath_draw_filled(ctx, hour_path);
    case  4 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 90 / 360);
      gpath_move_to(hour_path, GPoint(p2.x-1,p2.y));
      gpath_draw_filled(ctx, hour_path);
    case  3 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 270 / 360);
      gpath_move_to(hour_path, GPoint(p1.x+1,p1.y));
      gpath_draw_filled(ctx, hour_path);
    case  2 :
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 0 / 360);
      gpath_move_to(hour_path, GPoint(p1.x,p1.y+1));
      gpath_draw_filled(ctx, hour_path);
    case  1 : 
      gpath_rotate_to(hour_path, TRIG_MAX_ANGLE * 90 / 360);
      gpath_move_to(hour_path, GPoint(p1.x-1,p1.y));
      gpath_draw_filled(ctx, hour_path);
    default : break;
  }

  uint8_t gap = 144 / 11;
  for(uint8_t i=0; i<30; i++){
    graphics_draw_pixel(ctx, GPoint(gap * (1 + i % 10), gap * (1 + i / 10)));
    graphics_draw_pixel(ctx, GPoint(gap * (1 + i % 10), 168 - gap * (1 + i / 10)));
  }

  for(uint8_t i=0; i<minutes; i++){
    uint8_t tmp = i / 2;
    if(i % 2 == 0){
      graphics_fill_rect(ctx, GRect(gap * (1 + tmp % 10) - 2, gap * (1 + tmp / 10) - 2, 5, 5), 0, GCornerNone);
    } 
    else {
      graphics_fill_rect(ctx, GRect(144 - gap * (1 + tmp % 10) - 2, 168 - gap * (1 + tmp / 10) - 2, 5, 5), 0, GCornerNone);
    } 
  }

  uint8_t leftx = gap - 2;
  uint8_t rightx = 144 - 2 - 5*gap;

  if(minutes > 8){
    graphics_fill_rect(ctx, GRect(leftx, gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 9){
    graphics_fill_rect(ctx, GRect(rightx, 168 - gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 18){
    graphics_fill_rect(ctx, GRect(rightx, gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 19){
    graphics_fill_rect(ctx, GRect(leftx, 168 - gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 28){
    graphics_fill_rect(ctx, GRect(leftx, 2*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 29){
    graphics_fill_rect(ctx, GRect(rightx, 168 - 2*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 38){
    graphics_fill_rect(ctx, GRect(rightx, 2*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 39){
    graphics_fill_rect(ctx, GRect(leftx, 168 - 2*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }

  if(minutes > 48){
    graphics_fill_rect(ctx, GRect(leftx, 3*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 49){
    graphics_fill_rect(ctx, GRect(rightx, 168 - 3*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
  if(minutes > 58){
    graphics_fill_rect(ctx, GRect(rightx, 3*gap - 2, 5 + 4 * gap, 5), 0, GCornerNone);
  }
}

void seconds_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  if(seconds % 2 == 0){
    graphics_draw_line(ctx, GPoint(30,80), GPoint(114,80));
    if(hours >= 12){
      graphics_fill_rect(ctx, GRect(114 - 4, 77, 7, 7), 0, GCornerNone);
      graphics_draw_line(ctx, GPoint(30,80-3), GPoint(30,80+3));
    }
    else {
      graphics_fill_rect(ctx, GRect(30-7, 77, 7, 7), 0, GCornerNone);
      graphics_draw_line(ctx, GPoint(114,80-3), GPoint(114,80+3));
    }
    if(!bluetooth_connected){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_draw_line(ctx, GPoint(68,80), GPoint(74,80));
    }
  }
}

void handle_seconds_tick(struct tm *tick_time, TimeUnits units_changed) {
  if(hours != tick_time->tm_hour){
    hours = tick_time->tm_hour;
    layer_mark_dirty(minutes_layer);
  }
  if(minutes != tick_time->tm_min){
    minutes = tick_time->tm_min;
    layer_mark_dirty(minutes_layer);
  }
  if(seconds != tick_time->tm_sec){
    seconds = tick_time->tm_sec; 
    layer_mark_dirty(seconds_layer);
  }
}

void bluetooth_connection_handler(bool connected){
  if(connected != bluetooth_connected){
    bluetooth_connected = connected;
    layer_mark_dirty(seconds_layer);
  }
}

void handle_deinit(void) {
  gpath_destroy(hour_path);
  layer_destroy(minutes_layer);
  layer_destroy(seconds_layer);
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

void handle_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  GRect minutes_frame = GRect(0, 0, 144, 168);
  minutes_layer = layer_create(minutes_frame);
  layer_set_update_proc(minutes_layer, minutes_layer_update_callback);
  layer_add_child(window_layer, minutes_layer);

  GRect seconds_frame = GRect(0, 0, 144, 168);
  seconds_layer = layer_create(seconds_frame);
  layer_set_update_proc(seconds_layer, seconds_layer_update_callback);
  layer_add_child(window_layer, seconds_layer);

  hour_path = gpath_create(&HOUR_PATH_POINTS);

  time_t now;
  time(&now);
  tick_timer_service_subscribe(SECOND_UNIT, handle_seconds_tick);
  handle_seconds_tick(localtime(&now), SECOND_UNIT);

  bluetooth_connection_service_subscribe(bluetooth_connection_handler);
  bluetooth_connected = bluetooth_connection_service_peek();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}