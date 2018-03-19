#include <pebble.h>
#include "batt.h"

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