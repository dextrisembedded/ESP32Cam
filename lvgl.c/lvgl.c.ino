/* MODIFIED FOR CUSTOM ESP32 + GENERIC TFT SETUP 
    Based on the Random Nerd Tutorials template, updated for manual wiring.
*/

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// --- TFT BACKLIGHT PIN ---
// Based on your image, the LED pin is on GPIO 21
#define TFT_BL 21 

// --- TOUCHSCREEN PINS ---
// Mapped exactly to your image
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(HSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates
int x, y, z;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// LVGL Logging
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// Get the Touchscreen data
// Get the Touchscreen data
// Get the Touchscreen data
// Get the Touchscreen data (UPDATED & BULLETPROOFED)
// Get the Touchscreen data (Fixed for LVGL v9 Auto-Rotation)
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  if(touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    
    // DEBUG: Print the raw electrical data from the glass
    Serial.print("Raw X: "); Serial.print(p.x);
    Serial.print(" | Raw Y: "); Serial.print(p.y);

    // Map to PORTRAIT dimensions (Max indices are 239 and 319)
    x = map(p.x, 200, 3700, 0, 239);
    y = map(p.y, 240, 3800, 0, 319);
    
    // Constrain to maximum pixel bounds
    x = constrain(x, 0, 239);
    y = constrain(y, 0, 319);

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;

    Serial.print(" => Mapped X: "); Serial.print(x);
    Serial.print(" | Mapped Y: "); Serial.println(y);
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

// --- GUI CALLBACKS ---
int btn1_count = 0;
static void event_handler_btn1(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  if(code == LV_EVENT_CLICKED) {
    btn1_count++;
    LV_LOG_USER("Button clicked %d", (int)btn1_count);
  }
}

static void event_handler_btn2(lv_event_t * e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = (lv_obj_t*) lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    LV_UNUSED(obj);
    LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
  }
}

static lv_obj_t * slider_label;
static void slider_event_callback(lv_event_t * e) {
  lv_obj_t * slider = (lv_obj_t*) lv_event_get_target(e);
  char buf[8];
  lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
  lv_label_set_text(slider_label, buf);
  lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  LV_LOG_USER("Slider changed to %d%%", (int)lv_slider_get_value(slider));
}

// --- CREATE GUI ---
void lv_create_main_gui(void) {
  lv_obj_t * text_label = lv_label_create(lv_screen_active());
  lv_label_set_long_mode(text_label, LV_LABEL_LONG_WRAP);
  lv_label_set_text(text_label, "Hello, world!");
  lv_obj_set_width(text_label, 150);
  lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_align(text_label, LV_ALIGN_CENTER, 0, -90);

  lv_obj_t * btn1 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn1, event_handler_btn1, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -50);
  lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

  lv_obj_t * btn_label = lv_label_create(btn1);
  lv_label_set_text(btn_label, "Button");
  lv_obj_center(btn_label);

  lv_obj_t * btn2 = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn2, event_handler_btn2, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 10);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_height(btn2, LV_SIZE_CONTENT);

  btn_label = lv_label_create(btn2);
  lv_label_set_text(btn_label, "Toggle");
  lv_obj_center(btn_label);
  
  lv_obj_t * slider = lv_slider_create(lv_screen_active());
  lv_obj_align(slider, LV_ALIGN_CENTER, 0, 60);
  lv_obj_add_event_cb(slider, slider_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
  lv_slider_set_range(slider, 0, 100);
  lv_obj_set_style_anim_duration(slider, 2000, 0);

  slider_label = lv_label_create(lv_screen_active());
  lv_label_set_text(slider_label, "0%");
  lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void setup() {
  Serial.begin(115200);

  // 1. POWER ON THE BACKLIGHT
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  // Start LVGL
  lv_init();
  lv_log_register_print_cb(log_print);

  // Start SPI and Touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(2); // Change to 0 if touches are upside down

  // Initialize the TFT display
  lv_display_t * disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_270);
    
  // Initialize Input Device (Touchscreen)
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Draw GUI
  lv_create_main_gui();
}

// Variables for smooth timing
static uint32_t last_tick = 0;

void loop() {
  // Use millis() for accurate LVGL timing instead of blind delays
  uint32_t current_tick = millis();
  lv_tick_inc(current_tick - last_tick);
  last_tick = current_tick;

  lv_timer_handler(); // Updated from deprecated lv_task_handler()
  delay(5);           // Small delay keeps the ESP32 happy
}