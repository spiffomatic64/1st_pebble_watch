#include <pebble.h>
#include "settings.h"

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