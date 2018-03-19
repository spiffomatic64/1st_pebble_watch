#include <pebble.h>
#include "phone.h"

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

static void update_phone_batt() {
    
  dash_api_get_data(DataTypeBatteryPercent, get_callback);
  
}