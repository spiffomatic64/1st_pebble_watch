#include <pebble.h>
#include "main.h"
#include <stdio.h>
#include <ctype.h>

#include <pebble-events/pebble-events.h>
#include <pebble-dash-api/pebble-dash-api.h>

#include "health.h"

#include <time.h>

//todo: add weather, gsm strength, sms count, and calendar
//https://www.npmjs.com/package/pebble-dash-api

static const uint32_t KEY_SAVEDATE = 0x01;
static const uint32_t KEY_BT = 0x02;
static const uint32_t KEY_BATT = 0x03;
static const uint32_t KEY_PHONEBATT = 0x04;
static const uint32_t KEY_STEPS = 0x05;
  
static Window *s_main_window;

//Texts
static TextLayer *s_time_layer;
static TextLayer *current_date_layer;
static TextLayer *timephase_layer;
static TextLayer *steps_layer;

static GFont s_time_font;
static GFont s_timephase_font;
static GFont s_date_font;
static GFont s_steps_font;

//Bitmaps
static BitmapLayer *clock_layer;
static BitmapLayer *batt_layer;
static BitmapLayer *phone_layer;

static GBitmap *clock_bitmap;
static GBitmap *batt_bitmap;
static GBitmap *phone_bitmap;

//Canvass
static Layer *canvas_layer;

// Create a long-lived buffer
static char buffer[] = "00:00";
static char current_date_buffer[] = "00.00 000";
static char timephase_buffer[] = "00";
static char steps_text[] = "00000000";

static int phone_batt = 0;
static int watch_batt = 0;
static int current_steps = 0;

static bool bluetooth_connected = false;

static time_t savedTimestamp;

ClaySettings settings;

static void debug_print_all_values()
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
  APP_LOG(APP_LOG_LEVEL_INFO, "Phone Battery: %d", phone_batt);
  APP_LOG(APP_LOG_LEVEL_INFO, "Watch Battery: %d", watch_batt);
  APP_LOG(APP_LOG_LEVEL_INFO, "Steps: %d", current_steps);
  APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth: %d", bluetooth_connected);
  APP_LOG(APP_LOG_LEVEL_ERROR, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
}

static void error_callback(ErrorCode code) {
  //if(code != ErrorCodeSuccess) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error! Code=%d", code);
  //}
}

static void updatePhoneBatt( int level )
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Updating Phone Batt with %d",level); 
  
  gbitmap_destroy(phone_bitmap);
  
  if (phone_batt>90) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone100);
  else if (phone_batt>80) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone80);
  else if (phone_batt>70) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone70);
  else if (phone_batt>60) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone60);
  else if (phone_batt>50) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone50);
  else if (phone_batt>40) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone40);
  else if (phone_batt>30) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone30);
  else if (phone_batt>20) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone20);
  else if (phone_batt>10) phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone10);
  else phone_bitmap = gbitmap_create_with_resource(RESOURCE_ID_phone00);
       
  bitmap_layer_set_bitmap(phone_layer, phone_bitmap);
}

static void get_callback(DataType type, DataValue result) {
  APP_LOG(APP_LOG_LEVEL_INFO, "get_callback called: %d", result.integer_value);
  if (type == DataTypeBatteryPercent)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone Battery: %d%%", result.integer_value);
    if (phone_batt != result.integer_value)
    {
      phone_batt = result.integer_value;
      updatePhoneBatt(phone_batt);  
    }
  }
}

// Initialize the default settings
static void prv_default_settings() {
  settings.Bluetooth = true;
  settings.Animations = false;
}

// Read settings from persistent storage
static void prv_load_settings() {
  
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

//add more configs, that actually do stuff
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  
  // Bluetooth
  Tuple *bluetooth_t = dict_find(iter, MESSAGE_KEY_Bluetooth);
  if (bluetooth_t) {
    settings.Bluetooth = bluetooth_t->value->int32 == 1;
  }

  // Animations
  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_Animations);
  if (animations_t) {
    settings.Animations = animations_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

static void update_phone_batt() {
    
  dash_api_get_data(DataTypeBatteryPercent, get_callback);
  
}

void get_steps_data() {

    current_steps = (int)health_service_sum_today(HealthMetricStepCount);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data: %d", current_steps);

    //current_steps = 123456;
    snprintf(steps_text, sizeof(steps_text), "%d", current_steps);

}

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

static void bluetooth_handler(bool connected) {
  bluetooth_connected = connected;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX BT Status=%d", connected);
  vibes_double_pulse();
  layer_mark_dirty(canvas_layer);
}

static void updateBatt( int level )
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Updating Batt with %d",level); 
  gbitmap_destroy(batt_bitmap);
      
  if (level>=90) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt100);
  else if (level>=80) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt80);
  else if (level>=70) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt70);
  else if (level>=60) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt60);
  else if (level>=50) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt50);
  else if (level>=40) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt40);
  else if (level>=30) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt30);
  else if (level>=20) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt20);
  else if (level>=10) batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt10);
  else batt_bitmap = gbitmap_create_with_resource(RESOURCE_ID_batt00);

  bitmap_layer_set_bitmap(batt_layer, batt_bitmap);
}

static void battery_handler(BatteryChargeState charge_state) {
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "battery_handler", charge_state.charge_percent, charge_state.is_charging, charge_state.is_plugged);
    
    if (watch_batt != charge_state.charge_percent)
    {
      watch_batt = charge_state.charge_percent;
      APP_LOG(APP_LOG_LEVEL_INFO, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX charge_percent=%d is_charging=%d is_plugged=%d", charge_state.charge_percent, charge_state.is_charging, charge_state.is_plugged);
      
      updateBatt(charge_state.charge_percent);
    }
}

static void init() {
  
  prv_load_settings();

  // Listen for AppMessages for claysettings
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
    
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Dash setup
  dash_api_init(APP_NAME, error_callback);
  events_app_message_open();
  
  // Register with bluetooth service
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_handler
  });
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, min_handler);
  
  // Register with battery service
  battery_state_service_subscribe(battery_handler);
  
  // Register with health service
  health_init();
  
  //check if we should get first data manually, or from persistant storage
  bool manually_update = true;
  time_t currentTimestamp;
  time(&currentTimestamp);

  //check if there is persistant storage
  if (persist_exists(KEY_SAVEDATE) && persist_exists(KEY_BT) && persist_exists(KEY_BATT) &&
            persist_exists(KEY_PHONEBATT) && persist_exists(KEY_STEPS))
  {
    //get timestamp
    persist_read_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));

    int timediff = currentTimestamp - savedTimestamp;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX savedTimestamp %ld", savedTimestamp);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX currentTimestamp %ld", currentTimestamp);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX timediff %ld", timediff);
    
    //if storage is older than ... 5mins
    if (timediff <= 300)
    {
      manually_update = false;
    } 
  } else
  {
    APP_LOG(APP_LOG_LEVEL_ERROR, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX nokeys update!");
  }
  
  if (manually_update)
  {
      APP_LOG(APP_LOG_LEVEL_WARNING, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX timediff manually update!");
      battery_handler(battery_state_service_peek());
      update_phone_batt();
      bluetooth_connected = bluetooth_connection_service_peek();
      get_steps_data();  
      debug_print_all_values();
  } else 
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Reading cached values!");
    bluetooth_connected = persist_read_bool(KEY_BT);
    if (bluetooth_connected == false) bluetooth_connected = bluetooth_connection_service_peek();
    watch_batt = persist_read_int(KEY_BATT);
    updateBatt(watch_batt);
    phone_batt = persist_read_int(KEY_PHONEBATT);
    current_steps = persist_read_int(KEY_STEPS);
    snprintf(steps_text, sizeof(steps_text), "%d", current_steps);
    debug_print_all_values();
  }
             
  APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Initialized");
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
  
  //unsubscribe
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
  //save timestamp, bluetooth, battery, phone battery, and steps
  time_t currentTimestamp;
  time(&currentTimestamp);

  persist_write_data(KEY_SAVEDATE, (void*)&currentTimestamp, sizeof(currentTimestamp));
  persist_write_bool(KEY_BT, bluetooth_connected);
  persist_write_int(KEY_BATT, watch_batt);
  persist_write_int(KEY_PHONEBATT, phone_batt);
  persist_write_int(KEY_STEPS, current_steps);
  debug_print_all_values();
  APP_LOG(APP_LOG_LEVEL_INFO, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX De-Initialized saving timestamp: %ld",currentTimestamp);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

