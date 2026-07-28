#include <lvgl.h>
#include <TFT_eSPI.h>
#include "src/ui/ui.h"

// =====================================================
// DISPLAY SIZE
// =====================================================

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320


// =====================================================
// TFT OBJECT
// =====================================================

TFT_eSPI tft = TFT_eSPI();


// =====================================================
// LVGL DRAW BUFFER
// =====================================================

#define DRAW_BUF_SIZE \
    (SCREEN_WIDTH * 40 * (LV_COLOR_DEPTH / 8))

uint8_t draw_buf[DRAW_BUF_SIZE];


// =====================================================
// LVGL DISPLAY FLUSH FUNCTION
// =====================================================

void my_disp_flush(
    lv_display_t *disp,
    const lv_area_t *area,
    uint8_t *px_map
)
{
    // Check whether the drawing area is valid
    if (
        area->x2 < 0 ||
        area->y2 < 0 ||
        area->x1 >= SCREEN_WIDTH ||
        area->y1 >= SCREEN_HEIGHT
    )
    {
        lv_display_flush_ready(disp);
        return;
    }

    uint32_t width =
        area->x2 - area->x1 + 1;

    uint32_t height =
        area->y2 - area->y1 + 1;

    // Start SPI communication
    tft.startWrite();

    // Select the display area
    tft.setAddrWindow(
        area->x1,
        area->y1,
        width,
        height
    );

    // Send LVGL pixels to TFT
    tft.pushColors(
        (uint16_t *)px_map,
        width * height,
        false
    );

    // End SPI communication
    tft.endWrite();

    // Inform LVGL that drawing is complete
    lv_display_flush_ready(disp);
}


// =====================================================
// TOUCH READ FUNCTION
// =====================================================

// =====================================================
// TOUCH READ FUNCTION
// =====================================================

void my_touchpad_read(
    lv_indev_t *indev,
    lv_indev_data_t *data
)
{
    uint16_t touchX = 0;
    uint16_t touchY = 0;

    // Get the touch coordinates from the TFT controller
    bool touched = tft.getTouch(
        &touchX,
        &touchY
    );

    if (touched)
    {
        // Tell LVGL the screen is currently being pressed
        data->state = LV_INDEV_STATE_PRESSED;
        
        // ==========================================
        // FIX: SWAP THE X AND Y COORDINATES HERE
        // ==========================================
        data->point.x = touchY; 
        data->point.y = touchX;

        // Print touch values to the Serial Monitor for debugging
        Serial.print("TOUCH (Raw): X = ");
        Serial.print(touchX);
        Serial.print(" | Y = ");
        Serial.println(touchY); // <-- Double check this says touchY
    }
    else
    {
        // Tell LVGL the screen is released
        data->state = LV_INDEV_STATE_RELEASED;
    }
}


// =====================================================
// LVGL TICK FUNCTION
// =====================================================

static uint32_t my_tick()
{
    return millis();
}

// =====================================================
// EEZ CUSTOM BUTTON ACTIONS
// =====================================================

// =====================================================
// EEZ CUSTOM BUTTON ACTIONS
// =====================================================

// =====================================================
// EEZ CUSTOM BUTTON ACTIONS
// =====================================================

#ifdef __cplusplus
extern "C" {
#endif

void action_go_to_cam(lv_event_t *e)
{
    lv_screen_load(objects.cam);
}

void action_go_to_wifi(lv_event_t *e)
{
    Serial.println("WIFI BUTTON WAS CLICKED!");
    lv_screen_load(objects.wifi); 
}

void action_go_to_home(lv_event_t *e)
{
    lv_screen_load(objects.home); 
}

// Add this back in to satisfy the compiler for any buttons 
// you haven't updated in EEZ Studio yet!
void action_load_screen(lv_event_t *e)
{
    // You can leave this blank, or make it go home by default
    lv_screen_load(objects.home);
}

#ifdef __cplusplus
}
#endif


// =====================================================
// SETUP
// =====================================================

void setup()
{
    // =================================================
    // SERIAL MONITOR
    // =================================================

    Serial.begin(115200);
    delay(1000);

    Serial.println(
        "ESP32 TFT + LVGL + EEZ Started"
    );


    // =================================================
    // INITIALIZE TFT
    // =================================================

    tft.begin();

    // Set rotation to match your display configuration
    tft.setRotation(1);

    Serial.print("TFT Width: ");
    Serial.println(tft.width());

    Serial.print("TFT Height: ");
    Serial.println(tft.height());


    // =================================================
    // DISPLAY SETTINGS
    // =================================================

    // Required for LVGL RGB565 colors
    tft.setSwapBytes(true);

    // Keep because your display requires inversion
    tft.invertDisplay(true);

    // Clear display
    tft.fillScreen(TFT_BLACK);


    // =================================================
    // TOUCH CALIBRATION
    // =================================================

    // =================================================
    // TOUCH CALIBRATION (Standard Defaults)
    // =================================================

    uint16_t calData[5] = { 300, 3600, 300, 3600, 0 };

    tft.setTouch(calData);

    tft.setTouch(calData);

    Serial.println("Touch initialized");


    // =================================================
    // INITIALIZE LVGL
    // =================================================

    lv_init();

    lv_tick_set_cb(
        my_tick
    );


    // =================================================
    // CREATE LVGL DISPLAY
    // =================================================

    lv_display_t *disp =
        lv_display_create(
            SCREEN_WIDTH,
            SCREEN_HEIGHT
        );

    lv_display_set_flush_cb(
        disp,
        my_disp_flush
    );

    lv_display_set_buffers(
        disp,
        draw_buf,
        NULL,
        sizeof(draw_buf),
        LV_DISPLAY_RENDER_MODE_PARTIAL
    );


    // =================================================
    // CREATE LVGL TOUCH INPUT
    // =================================================

    lv_indev_t *indev = lv_indev_create();

    lv_indev_set_type(
        indev,
        LV_INDEV_TYPE_POINTER
    );

    lv_indev_set_read_cb(
        indev,
        my_touchpad_read
    );

    // Connect touch to LVGL display
    lv_indev_set_display(
        indev,
        disp
    );


    // =================================================
    // INITIALIZE EEZ UI
    // =================================================

    ui_init();

    Serial.println("UI Loaded Successfully");
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
    // 1. Process EEZ Studio screen logic and custom actions
    ui_tick(); 

    // 2. Process LVGL graphics and touch inputs
    lv_timer_handler(); 
    
    delay(5);
}
