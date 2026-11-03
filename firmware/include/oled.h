#ifndef MACROPAD_OLED_H
#define MACROPAD_OLED_H

#include <cstddef>
#include <cstdint>
#include "config.h"

namespace macropad {

class OledDisplay {
public:
    OledDisplay() = default;

    // Initialize I2C0 on GP8 (SDA) / GP9 (SCL) and configure SSD1306 128x64 panel.
    // Returns true if the display ACKed over I2C.
    bool init();

    bool is_connected() const { return connected_; }

    // Clear internal 1024-byte framebuffer
    void clear();

    // Draw an ASCII string at pixel coordinates (x, page*8) using 5x7 bitmap font
    void draw_text(uint8_t x, uint8_t page, const char* text, bool invert = false);

    // Render complete HUD showing 3x3 key matrix live state, last action, and USB state
    void render_status(uint16_t key_mask, const char* last_action, bool usb_mounted);

    // Flush 1024-byte framebuffer to SSD1306 over I2C
    void flush();

private:
    bool send_command(uint8_t cmd);
    bool send_command_list(const uint8_t* cmds, size_t len);

    bool    connected_ = false;
    uint8_t buffer_[OLED_WIDTH * (OLED_HEIGHT / 8)]{};
};

} // namespace macropad

#endif // MACROPAD_OLED_H
