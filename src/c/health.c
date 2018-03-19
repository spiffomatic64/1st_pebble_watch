#include "health.h"
#include "main.h"

static void health_handler(HealthEventType event, void *context) {
  if(event == HealthEventMovementUpdate) get_steps_data();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Health data: %d", event);
}

void health_init() {
  health_service_events_subscribe(health_handler, NULL);
}