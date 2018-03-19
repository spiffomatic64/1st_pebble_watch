#pragma once

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

static void update_time();

static void set_text_to_window();

static void canvas_update_proc(Layer *layer, GContext *ctx);

static void main_window_load(Window *window);

static void main_window_unload(Window *window);

static void min_handler(struct tm *tick_time, TimeUnits units_changed);