#include "health.h"
#include "main.h"

static void health_handler(HealthEventType event, void *context) {
  if(event == HealthEventMovementUpdate) get_steps_data();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Health data: %d", event);
}

void health_init() {
  health_service_events_subscribe(health_handler, NULL);
}

void get_steps_data() {

    current_steps = (int)health_service_sum_today(HealthMetricStepCount);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Steps data: %d", current_steps);

    //current_steps = 123456;
    snprintf(steps_text, sizeof(steps_text), "%d", current_steps);

}