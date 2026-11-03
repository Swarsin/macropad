#include <cstring>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "bsp/board.h"
#include "tusb.h"

#include "config.h"
#include "usb_descriptors.h"
#include "matrix.h"
#include "encoder.h"
#include "oled.h"

using namespace macropad;

namespace {

KeyMatrix     g_matrix;
RotaryEncoder g_encoder;
OledDisplay   g_oled;

bool        g_keyboard_report_pending = false;
uint16_t    g_pending_consumer_code   = 0;
bool        g_consumer_release_pending = false;
const char* g_last_action_label       = "READY";
bool        g_oled_dirty              = true;

void send_keyboard_hid_report() {
    if (!tud_hid_ready()) return;

    uint8_t keycodes[6] = {0};
    uint8_t count = 0;

    for (uint8_t r = 0; r < MATRIX_ROWS && count < 6; ++r) {
        for (uint8_t c = 0; c < MATRIX_COLS && count < 6; ++c) {
            if (g_matrix.is_pressed(r, c)) {
                keycodes[count++] = KEYMAP[r][c];
            }
        }
    }

    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycodes);
    g_keyboard_report_pending = false;
}

void service_consumer_hid_report() {
    if (!tud_hid_ready()) return;

    if (g_pending_consumer_code != 0) {
        uint16_t code = g_pending_consumer_code;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &code, sizeof(code));
        g_pending_consumer_code = 0;
        g_consumer_release_pending = true;
    } else if (g_consumer_release_pending) {
        uint16_t release_code = 0;
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &release_code, sizeof(release_code));
        g_consumer_release_pending = false;
    }
}

} // namespace

int main() {
    board_init();
    tusb_init();

    g_matrix.init();
    g_encoder.init();
    g_oled.init();

    uint64_t last_scan_us = time_us_64();
    uint64_t last_oled_us = last_scan_us;
    bool     last_usb_mounted = false;

    while (true) {
        // Service TinyUSB device stack
        tud_task();

        const uint64_t now_us = time_us_64();

        // Poll high-rate rotary encoder quadrature & button
        g_encoder.update(now_us);

        // 1 kHz matrix scan and debounce update
        if ((now_us - last_scan_us) >= SCAN_INTERVAL_US) {
            last_scan_us = now_us;

            if (g_matrix.scan(now_us)) {
                g_keyboard_report_pending = true;
                g_oled_dirty = true;

                // Record the most recently pressed key for OLED HUD
                for (uint8_t r = 0; r < MATRIX_ROWS; ++r) {
                    for (uint8_t c = 0; c < MATRIX_COLS; ++c) {
                        if (g_matrix.rose(r, c)) {
                            g_last_action_label = KEY_LABELS[r][c];
                        }
                    }
                }

                // Wake host if suspended
                if (tud_suspended()) {
                    tud_remote_wakeup();
                }
            }
        }

        // Check rotary encoder rotation (Volume Up / Volume Down) & switch (Mute)
        const int8_t enc_delta = g_encoder.take_delta();
        if (enc_delta > 0) {
            g_pending_consumer_code = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
            g_last_action_label = "VOL UP";
            g_oled_dirty = true;
        } else if (enc_delta < 0) {
            g_pending_consumer_code = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
            g_last_action_label = "VOL DN";
            g_oled_dirty = true;
        } else if (g_encoder.take_button_press()) {
            g_pending_consumer_code = HID_USAGE_CONSUMER_MUTE;
            g_last_action_label = "MUTE";
            g_oled_dirty = true;
        }

        // Transmit pending USB HID Keyboard & Consumer Control reports
        if (g_keyboard_report_pending) {
            send_keyboard_hid_report();
        } else {
            service_consumer_hid_report();
        }

        // Refresh 128x64 I2C OLED display at up to 20 Hz when state changes
        const bool usb_mounted = tud_mounted();
        if (usb_mounted != last_usb_mounted) {
            last_usb_mounted = usb_mounted;
            g_oled_dirty = true;
        }

        if (g_oled_dirty && (now_us - last_oled_us) >= OLED_REFRESH_US) {
            last_oled_us = now_us;
            g_oled_dirty = false;
            g_oled.render_status(g_matrix.pressed_mask(), g_last_action_label, usb_mounted);
        }
    }

    return 0;
}

//--------------------------------------------------------------------+
// TinyUSB HID Callbacks
//--------------------------------------------------------------------+
extern "C" void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) {
    (void)instance;
    (void)report;
    (void)len;
    service_consumer_hid_report();
}

extern "C" uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

extern "C" void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
