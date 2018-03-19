#include <pebble.h>
#include "ui.h"

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "UPDATE TIME");
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    strftime(timephase_buffer, sizeof("00"), "  ", tick_time);
      
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    strftime(timephase_buffer, sizeof("00"), "%p", tick_time);
  }       
  
  strftime(current_date_buffer, sizeof("00.00 000"), "%m.%d %a", tick_time);

  int i=0;
  char c;
  while (current_date_buffer[i])
  {
    c = current_date_buffer[i];
    current_date_buffer[i] = toupper((unsigned char)c);
    i++;
  }  //write to all text layers

  
    
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(timephase_layer, timephase_buffer);
  text_layer_set_text(current_date_layer, current_date_buffer);
  text_layer_set_text(steps_layer, steps_text);
}

static void set_text_to_window() {

  //Time TextLayer 
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRYSTAL_48));
  s_time_layer = text_layer_create(GRect(25, 50, 130, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  //Time TextLayer 
  s_timephase_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRYSTAL_18));
  timephase_layer = text_layer_create(GRect(115, 95, 40, 40));
  text_layer_set_background_color(timephase_layer, GColorClear);
  text_layer_set_text_color(timephase_layer, GColorWhite);
  text_layer_set_text(timephase_layer, "00");
  text_layer_set_font(timephase_layer, s_timephase_font);
  text_layer_set_text_alignment(timephase_layer, GTextAlignmentCenter);
  
  // Create current date TextLayer 
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRYSTAL_18));
  current_date_layer = text_layer_create(GRect(32, 153, 120, 30));
  text_layer_set_background_color(current_date_layer, GColorClear);
  text_layer_set_text_color(current_date_layer, GColorWhite);
  text_layer_set_text(current_date_layer, "00.00 000");
  text_layer_set_font(current_date_layer, s_date_font);
  text_layer_set_text_alignment(current_date_layer, GTextAlignmentCenter);
  
  //Steps TextLayer 
  s_steps_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRYSTAL_24));
  steps_layer = text_layer_create(GRect(30, 5, 120, 30));
  text_layer_set_background_color(steps_layer, GColorClear);
  text_layer_set_text_color(steps_layer, GColorWhite);
  text_layer_set_text(steps_layer, "00000000");
  text_layer_set_font(steps_layer, s_steps_font);
  text_layer_set_text_alignment(steps_layer, GTextAlignmentCenter);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  if(!bluetooth_connected) {
    graphics_context_set_fill_color(ctx, GColorRed);
    graphics_fill_rect(ctx, GRect(49, 132, 15, 3), 0, GCornerNone);
  }
}

static void main_window_load(Window *window) {
  
  // Get the Window's root layer
  Layer *root_layer = window_get_root_layer(window);
  
  //ACTION: Create GBitmap, then set to created BitmapLayer
  clock_bitmap = gbitmap_create_with_resource(RESOURCE_ID_bg_image);
  clock_layer = bitmap_layer_create(GRect(0, 0, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT));
  bitmap_layer_set_bitmap(clock_layer, clock_bitmap);
  layer_add_child(root_layer, bitmap_layer_get_layer(clock_layer));

  //BATTERY: Create GBitmap, then set to created BitmapLayer
  batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt100);
  batt_layer = bitmap_layer_create(GRect(0 ,0, 55, PBL_DISPLAY_HEIGHT));
  bitmap_layer_set_bitmap(batt_layer, batt_bitmap);
  layer_add_child(root_layer, bitmap_layer_get_layer(batt_layer));
  
    //BATTERY: Create GBitmap, then set to created BitmapLayer
  phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone100);
  phone_layer = bitmap_layer_create(GRect( PBL_DISPLAY_WIDTH - 55, 0, 55, PBL_DISPLAY_HEIGHT));
  bitmap_layer_set_bitmap(phone_layer, phone_bitmap);
  layer_add_child(root_layer, bitmap_layer_get_layer(phone_layer));
  

  //Create text layers
  set_text_to_window();
  layer_add_child(root_layer, text_layer_get_layer(current_date_layer));
  layer_add_child(root_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(root_layer, text_layer_get_layer(timephase_layer));
  layer_add_child(root_layer, text_layer_get_layer(steps_layer));
  
  // Create canvas layer
  GRect bounds = layer_get_bounds(window_get_root_layer(window));
  canvas_layer = layer_create(bounds);

  // Assign the custom drawing procedure
  layer_set_update_proc(canvas_layer, canvas_update_proc);
  
  // Add the layer, publish the window
  layer_add_child(root_layer, canvas_layer);
  
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
    
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_timephase_font);
  fonts_unload_custom_font(s_date_font);
  fonts_unload_custom_font(s_steps_font);

  //Destroy GBitmap
  gbitmap_destroy(clock_bitmap);
  gbitmap_destroy(batt_bitmap);
  gbitmap_destroy(phone_bitmap);

  //Destroy BitmapLayer
  bitmap_layer_destroy(clock_layer);
  bitmap_layer_destroy(batt_layer);
  bitmap_layer_destroy(phone_layer);
  
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(timephase_layer);
  text_layer_destroy(current_date_layer);
  text_layer_destroy(steps_layer);
}

static void min_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();  
  if(tick_time->tm_min % 10 == 0) update_phone_batt();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Min Handler");
}