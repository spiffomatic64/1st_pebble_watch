#include <pebble.h>
#include "main.h"
#include <stdio.h>
#include <ctype.h>

#include <pebble-events/pebble-events.h>
#include <pebble-dash-api/pebble-dash-api.h>

#include "health.h"
#include "bluetooth.h"
#include "phone.h"
#include "ui.h"
#include "settings.h"
#include "batt.h"

#include <time.h>

//todo: add weather, gsm strength, sms count, and calendar
//https://www.npmjs.com/package/pebble-dash-api

static const uint32_t KEY_SAVEDATE = 0x01;
static const uint32_t KEY_BT = 0x02;
static const uint32_t KEY_BATT = 0x03;
static const uint32_t KEY_PHONEBATT = 0x04;
static const uint32_t KEY_STEPS = 0x05;

static int phone_batt = 0;
static int watch_batt = 0;
static int current_steps = 0;

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

