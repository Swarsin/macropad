#ifndef MACROPAD_CONFIG_H
#define MACROPAD_CONFIG_H

#include <cstdint>
#include "tusb.h"

namespace macropad {

// 3x3 key matrix
// Diodes are COL2ROW: anode on the column/switch side, cathode on the row net.
constexpr uint8_t MATRIX_ROWS = 3;
constexpr uint8_t MATRIX_COLS = 3;
constexpr uint8_t NUM_KEYS    = MATRIX_ROWS * MATRIX_COLS;

// Row GPIOs: Row 0 -> GP0, Row 1 -> GP1, Row 2 -> GP2
constexpr uint8_t ROW_PINS[MATRIX_ROWS] = {0, 1, 2};

// Column GPIOs: Column 0 -> GP3, Column 1 -> GP4, Column 2 -> GP5
constexpr uint8_t COL_PINS[MATRIX_COLS] = {3, 4, 5};

// Matrix scanning & per-key debouncing parameters
constexpr uint32_t SCAN_INTERVAL_US   = 1000;  // 1 kHz (1 ms) scan rate
constexpr uint32_t SETTLE_DELAY_US    = 5;     // 5 us column-to-row diode settling delay
constexpr uint32_t DEBOUNCE_TIME_US   = 5000;  // 5 ms per-key symmetric debounce window

// Default 3x3 Keymap (S1-S9 mapped to HID keycodes '1' through '9')
constexpr uint8_t KEYMAP[MATRIX_ROWS][MATRIX_COLS] = {
    { HID_KEY_1, HID_KEY_2, HID_KEY_3 },  // Row 0: S1, S2, S3
    { HID_KEY_4, HID_KEY_5, HID_KEY_6 },  // Row 1: S4, S5, S6
    { HID_KEY_7, HID_KEY_8, HID_KEY_9 }   // Row 2: S7, S8, S9
};

// OLED labels
constexpr const char* KEY_LABELS[MATRIX_ROWS][MATRIX_COLS] = {
    { "S1 [Key 1]", "S2 [Key 2]", "S3 [Key 3]" },
    { "S4 [Key 4]", "S5 [Key 5]", "S6 [Key 6]" },
    { "S7 [Key 7]", "S8 [Key 8]", "S9 [Key 9]" }
};

// Rotary encoder
constexpr uint8_t ENCODER_PIN_A  = 17;
constexpr uint8_t ENCODER_PIN_B  = 16;
constexpr uint8_t ENCODER_PIN_SW = 18;
constexpr uint32_t ENCODER_SW_DEBOUNCE_US = 10000; // 10 ms switch debounce

// SSD1306 128x64 I2C OLED
constexpr uint8_t  OLED_SDA_PIN     = 8;
constexpr uint8_t  OLED_SCL_PIN     = 9;
constexpr uint8_t  OLED_I2C_ADDR    = 0x3C;
constexpr uint32_t OLED_I2C_BAUD    = 400000; // 400 kHz Fast-Mode I2C
constexpr uint8_t  OLED_WIDTH       = 128;
constexpr uint8_t  OLED_HEIGHT      = 64;
constexpr uint32_t OLED_REFRESH_US  = 50000;  // 20 Hz max screen refresh

} // namespace macropad

#endif // MACROPAD_CONFIG_H
