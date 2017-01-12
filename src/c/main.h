#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1

#define APP_NAME "Double0Spiff"

// A structure containing our settings
typedef struct ClaySettings {
  bool Bluetooth;
  bool Animations;
} __attribute__((__packed__)) ClaySettings;

void get_steps_data();