#include <pebble.h>
  
#define KEY_C_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_F_TEMPERATURE 2

Window *my_window;
TextLayer *top_text_layer;
TextLayer *bottom_text_layer;
TextLayer *date_text_layer;

static BitmapLayer *weather_icon_layer;
static GBitmap *weather_icon_bitmap;

static GFont time_font;
static GFont date_font;
static GFont weather_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer2[] =  "01 JAN";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  strftime(buffer2, sizeof("01 JAN"), "%d %b", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(top_text_layer, buffer);
  text_layer_set_text(date_text_layer, buffer2);
}

static void main_window_load(Window *window) {
  date_text_layer = text_layer_create(GRect(0, 0, 144, 28));
  text_layer_set_background_color(date_text_layer, GColorBlack);
  text_layer_set_text_color(date_text_layer, GColorWhite);
  text_layer_set_text_alignment(date_text_layer, GTextAlignmentCenter); 
  date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MASS_EFFECT_22));
  text_layer_set_font(date_text_layer, date_font);
  text_layer_set_text(date_text_layer, "01 JAN"); 
  
  top_text_layer = text_layer_create(GRect(0, 28, 144, 56));
  text_layer_set_background_color(top_text_layer, GColorBlack);
  text_layer_set_text_color(top_text_layer, GColorWhite);
  text_layer_set_text_alignment(top_text_layer, GTextAlignmentCenter); 
  time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MASS_EFFECT_30));
  text_layer_set_font(top_text_layer, time_font);
  text_layer_set_text(top_text_layer, "00:00");
  
  bottom_text_layer = text_layer_create(GRect(60, 100, 104, 84));
  text_layer_set_background_color(bottom_text_layer, GColorClear);
  text_layer_set_text_color(bottom_text_layer, GColorBlack);
  text_layer_set_text_alignment(bottom_text_layer, GTextAlignmentLeft); 
  weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MASS_EFFECT_24));
  text_layer_set_font(bottom_text_layer, weather_font);
  text_layer_set_text(bottom_text_layer, "Loading...");
  
  weather_icon_layer = bitmap_layer_create(GRect(5, 101, 50, 50));
  bitmap_layer_set_bitmap(weather_icon_layer, weather_icon_bitmap);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(top_text_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(bottom_text_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(weather_icon_layer));
  
  update_time();
}

static void main_window_unload(Window *window) {
  text_layer_destroy(date_text_layer);
  text_layer_destroy(top_text_layer);
  text_layer_destroy(bottom_text_layer);
  fonts_unload_custom_font(date_font);
  fonts_unload_custom_font(time_font);
  fonts_unload_custom_font(weather_font);
  gbitmap_destroy(weather_icon_bitmap);
  bitmap_layer_destroy(weather_icon_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char C_temperature_buffer[8];
  static char F_temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_C_TEMPERATURE:
      snprintf(C_temperature_buffer, sizeof(C_temperature_buffer), "%d°C", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s" ,t->value->cstring); 
      break;
    case KEY_F_TEMPERATURE:
      snprintf(F_temperature_buffer, sizeof(F_temperature_buffer), "%d°F", (int)t->value->int32);
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
    
    if (strcmp(conditions_buffer,"01d")==0)   {
        weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_01d);
    }
    else if (strcmp(conditions_buffer,"01n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_01n);
    }
    else if (strcmp(conditions_buffer,"02d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_02d);
    }
    else if (strcmp(conditions_buffer,"02n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_02n);
    }
    else if (strcmp(conditions_buffer,"03d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_03d);
    }
    else if (strcmp(conditions_buffer,"03n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_03n);
    }
    else if (strcmp(conditions_buffer,"04d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_03d);
    }
    else if (strcmp(conditions_buffer,"04n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_03n);
    }
    else if (strcmp(conditions_buffer,"09d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_09d);
    }
    else if (strcmp(conditions_buffer,"09n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_09n);
    }
    else if (strcmp(conditions_buffer,"10d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_10d);
    }
    else if (strcmp(conditions_buffer,"10n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_10n);
    }
    else if (strcmp(conditions_buffer,"11d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_11d);
    }
    else if (strcmp(conditions_buffer,"11n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_11n);
    }
    else if (strcmp(conditions_buffer,"13d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_13d);
    }
    else if (strcmp(conditions_buffer,"13n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_13n);
    }
    else if (strcmp(conditions_buffer,"50d")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_50d);
    }
    else if (strcmp(conditions_buffer,"50n")==0) {
      weather_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_50n);
    }
    
    
    
    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s\n%s", C_temperature_buffer, F_temperature_buffer);
    text_layer_set_text(bottom_text_layer, weather_layer_buffer);
    bitmap_layer_set_bitmap(weather_icon_layer, weather_icon_bitmap);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void handle_init(void) {
  
  my_window = window_create();
  
  window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(my_window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
