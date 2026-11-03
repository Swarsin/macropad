#include "oled.h"
#include "font5x7.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <cstring>

namespace macropad {

bool OledDisplay::send_command(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    int ret = i2c_write_timeout_us(i2c0, OLED_I2C_ADDR, buf, 2, false, 2000);
    return ret == 2;
}

bool OledDisplay::send_command_list(const uint8_t* cmds, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (!send_command(cmds[i])) return false;
    }
    return true;
}

bool OledDisplay::init() {
    i2c_init(i2c0, OLED_I2C_BAUD);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);

    static constexpr uint8_t init_seq[] = {
        0xAE,             // Display OFF
        0x20, 0x00,       // Memory Addressing Mode: Horizontal
        0x40,             // Display Start Line = 0
        0xA1,             // Segment Re-map (column 127 mapped to SEG0)
        0xC8,             // COM Output Scan Direction: remapped
        0xA8, 0x3F,       // Multiplex Ratio = 64MUX
        0xD3, 0x00,       // Display Offset = 0
        0xD5, 0x80,       // Display Clock Divide Ratio
        0xD9, 0xF1,       // Pre-charge Period
        0xDA, 0x12,       // COM Pins Hardware Configuration
        0xDB, 0x40,       // VCOMH Deselect Level
        0x81, 0xCF,       // Contrast Control
        0x8D, 0x14,       // Charge Pump Enable
        0xA4,             // Entire Display ON (resume to RAM content)
        0xA6,             // Normal Display (non-inverted)
        0xAF              // Display ON
    };

    connected_ = send_command_list(init_seq, sizeof(init_seq));
    if (connected_) {
        clear();
        flush();
    }
    return connected_;
}

void OledDisplay::clear() {
    std::memset(buffer_, 0, sizeof(buffer_));
}

void OledDisplay::draw_text(uint8_t x, uint8_t page, const char* text, bool invert) {
    if (page >= (OLED_HEIGHT / 8) || !text) return;
    uint16_t offset = static_cast<uint16_t>(page) * OLED_WIDTH + x;

    while (*text && (x + 6) <= OLED_WIDTH) {
        char c = *text++;
        if (c >= 'a' && c <= 'z') {
            c = static_cast<char>(c - ('a' - 'A'));
        }
        if (c < 0x20 || c > 0x5F) {
            c = ' ';
        }
        const uint8_t* glyph = FONT_5X7[c - 0x20];
        for (uint8_t col = 0; col < 5; ++col) {
            buffer_[offset + col] = invert ? static_cast<uint8_t>(~glyph[col]) : glyph[col];
        }
        buffer_[offset + 5] = invert ? 0xFF : 0x00;
        offset += 6;
        x += 6;
    }
}

void OledDisplay::render_status(uint16_t key_mask, const char* last_action, bool usb_mounted) {
    if (!connected_) return;
    clear();

    // Header bar
    draw_text(0, 0, "RP2040 MACROPAD", false);
    draw_text(98, 0, usb_mounted ? "USB:OK" : "USB:--", false);

    // Draw 3x3 visual matrix indicator on the left (pages 2..6)
    for (uint8_t r = 0; r < MATRIX_ROWS; ++r) {
        char row_str[12] = "[ ] [ ] [ ]";
        for (uint8_t c = 0; c < MATRIX_COLS; ++c) {
            bool pressed = (key_mask & (1u << (r * MATRIX_COLS + c))) != 0;
            row_str[c * 4 + 1] = pressed ? '#' : static_cast<char>('1' + r * 3 + c);
        }
        row_str[11] = '\0';
        draw_text(0, static_cast<uint8_t>(2 + r * 2), row_str, false);
    }

    // Draw last action / key event on the right side
    draw_text(72, 2, "LAST:", false);
    draw_text(72, 4, last_action ? last_action : "READY", false);
    flush();
}

void OledDisplay::flush() {
    if (!connected_) return;

    static constexpr uint8_t addr_cmds[] = {
        0x21, 0x00, OLED_WIDTH - 1,         // Column address range 0..127
        0x22, 0x00, (OLED_HEIGHT / 8) - 1   // Page address range 0..7
    };
    if (!send_command_list(addr_cmds, sizeof(addr_cmds))) {
        connected_ = false;
        return;
    }

    uint8_t tx_buf[1 + 32];
    tx_buf[0] = 0x40; // Co = 0, D/C# = 1 (GDDRAM data stream)

    for (size_t offset = 0; offset < sizeof(buffer_); offset += 32) {
        std::memcpy(&tx_buf[1], &buffer_[offset], 32);
        if (i2c_write_timeout_us(i2c0, OLED_I2C_ADDR, tx_buf, sizeof(tx_buf), false, 3000) < 0) {
            connected_ = false;
            return;
        }
    }
}

} // namespace macropad
