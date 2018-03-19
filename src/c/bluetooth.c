#include <pebble.h>
#include "bluetooth.h"

static void bluetooth_handler(bool connected) {
  bluetooth_connected = connected;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX BT Status=%d", connected);
  vibes_double_pulse();
  layer_mark_dirty(canvas_layer);
}