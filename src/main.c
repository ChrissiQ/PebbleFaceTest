#include <pebble.h>
#include "main.h"
  
#define TIME_TEMPLATE "00:00"
#define DATE_TEMPLATE "Sept 31"
#define FONT_SECONDS FONT_KEY_BITHAM_30_BLACK
#define FONT_MINUTES FONT_KEY_BITHAM_42_BOLD

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font;
static GFont s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // Create a long-lived buffer
  static char buffer[] = TIME_TEMPLATE;
  
  // Write the current hours and minutes into the buffer
  if (clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof(TIME_TEMPLATE), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof(TIME_TEMPLATE), "%I:%M", tick_time);
  }
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  
  static char buffer2[] = DATE_TEMPLATE;
  
  strftime(buffer2, sizeof(DATE_TEMPLATE), "%b %d", tick_time);
  text_layer_set_text(s_date_layer, buffer2);
}

static void update_date() {
//   time_t temp = time(NULL);
//   struct tm *tick_time = localtime(&temp);
  
//   static char buffer[] = " ";
  
//   strftime(buffer, sizeof(" Sept 31 "), "%b %d", tick_time);
//   text_layer_set_text(s_date_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void date_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_date();
}
  
static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, TIME_TEMPLATE);
  
  // Create date TextLayer
  s_date_layer = text_layer_create(GRect(5, 10, 139, 40));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, DATE_TEMPLATE);
  
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_32));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  
  // Add it as a child later to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Make sure time is displayed from the start
  update_time();
  update_date();
  
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
//   tick_timer_service_subscribe(DAY_UNIT, date_tick_handler);
}

static void deinit() {
  // Destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}