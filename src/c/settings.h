#pragma once

// Initialize the default settings
void prv_default_settings();

// Read settings from persistent storage
void prv_load_settings();

// Save the settings to persistent storage
void prv_save_settings();

//add more configs, that actually do stuff
void prv_inbox_received_handler(DictionaryIterator *iter, void *context);