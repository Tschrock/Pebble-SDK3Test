#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_layer2;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


struct BGData {
  uint32_t color;
  uint32_t black;
  uint32_t white;
  int8_t clock1x;
  int8_t clock1y;
  int8_t clock2x;
  int8_t clock2y;
};

static bool blackBG = false;
static int8_t bgImg = 0;
static struct BGData bgData[2] = {
                  { RESOURCE_ID_IMAGE_BG_0_COLOR, RESOURCE_ID_IMAGE_BG_0_BAW_BLACK, RESOURCE_ID_IMAGE_BG_0_BAW_WHITE, 88, 6, 88, 39 },
                  { RESOURCE_ID_IMAGE_BG_1_COLOR, RESOURCE_ID_IMAGE_BG_1_BAW_BLACK, RESOURCE_ID_IMAGE_BG_1_BAW_WHITE, 88, 6, 88, 39 }
};



#ifdef PBL_BW
  static GBitmap *s_background_bitmap2;
  static BitmapLayer *s_background_layer2;
#endif


static void checkBt_(bool connected) {
  if (connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is connected!");
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "Phone is not connected!");
  }
}

static void checkBt() {
  checkBt_(bluetooth_connection_service_peek());
}
  
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00";
  static char buffer2[] = "00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00"), "%H", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00"), "%I", tick_time);
  }
    strftime(buffer2, sizeof("00"), "%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_time_layer2, buffer2);
}

static void main_window_load(Window *window) {
  #ifdef PBL_COLOR
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_0_COLOR);
  #else
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_0_BAW_BLACK);
  #endif
    
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
    
  #ifdef PBL_COLOR
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);
  #else
    bitmap_layer_set_compositing_mode(s_background_layer, GCompOpClear);
  #endif
    
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
  #ifdef PBL_BW
    // Create GBitmap, then set to created BitmapLayer
    s_background_bitmap2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG_0_BAW_WHITE);
    s_background_layer2 = bitmap_layer_create(GRect(0, 0, 144, 168));
    
    bitmap_layer_set_compositing_mode(s_background_layer2, GCompOpOr);
    bitmap_layer_set_bitmap(s_background_layer2, s_background_bitmap2);
  
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer2));
  #endif
  
  ResHandle font_handle = resource_get_handle(RESOURCE_ID_ROBOTO_BOLD_40);
  
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(bgData[bgImg].clock1x, bgData[bgImg].clock1y, 54, 40));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  // Create time TextLayer2
  s_time_layer2 = text_layer_create(GRect(bgData[bgImg].clock2x, bgData[bgImg].clock2y, 54, 40));
  text_layer_set_background_color(s_time_layer2, GColorClear);
  text_layer_set_text_color(s_time_layer2, GColorBlack);

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer2, fonts_load_custom_font(font_handle));
  text_layer_set_text_alignment(s_time_layer2, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer2));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);

  #ifdef PBL_BW
    gbitmap_destroy(s_background_bitmap2);
    bitmap_layer_destroy(s_background_layer2);
  #endif
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  if(blackBG){
    window_set_background_color(s_main_window, GColorBlack);
  }
  else {
    window_set_background_color(s_main_window, GColorWhite);
  }
    
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
update_time();
  checkBt();
  
  // Register with TickTimerService
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  bluetooth_connection_service_subscribe(checkBt_);
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}