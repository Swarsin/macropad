#include "encoder.h"
#include "config.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

namespace macropad {

// Gray-code quadrature state transition lookup table.
// Rejects invalid contact bounce transitions by assigning 0 step weight.
static constexpr int8_t QUAD_TABLE[16] = {
     0, -1, +1,  0,
    +1,  0,  0, -1,
    -1,  0,  0, +1,
     0, +1, -1,  0
};

void RotaryEncoder::init() {
    gpio_init(ENCODER_PIN_A);
    gpio_set_dir(ENCODER_PIN_A, GPIO_IN);
    gpio_pull_up(ENCODER_PIN_A);

    gpio_init(ENCODER_PIN_B);
    gpio_set_dir(ENCODER_PIN_B, GPIO_IN);
    gpio_pull_up(ENCODER_PIN_B);

    gpio_init(ENCODER_PIN_SW);
    gpio_set_dir(ENCODER_PIN_SW, GPIO_IN);
    gpio_pull_up(ENCODER_PIN_SW);

    const uint8_t a = gpio_get(ENCODER_PIN_A) ? 1u : 0u;
    const uint8_t b = gpio_get(ENCODER_PIN_B) ? 1u : 0u;
    ab_state_ = static_cast<uint8_t>((a << 1) | b);
}

void RotaryEncoder::update(uint64_t now_us) {
    // 1. Decode quadrature channels A (GP17) and B (GP16)
    const uint8_t a = gpio_get(ENCODER_PIN_A) ? 1u : 0u;
    const uint8_t b = gpio_get(ENCODER_PIN_B) ? 1u : 0u;
    ab_state_ = static_cast<uint8_t>(((ab_state_ << 2) | (a << 1) | b) & 0x0Fu);

    quarter_steps_ += QUAD_TABLE[ab_state_];
    if (quarter_steps_ >= 4) {
        detent_delta_ += 1;
        quarter_steps_ = 0;
    } else if (quarter_steps_ <= -4) {
        detent_delta_ -= 1;
        quarter_steps_ = 0;
    }

    // 2. Debounce active-low push switch S2 (GP18)
    const bool pressed_sample = !gpio_get(ENCODER_PIN_SW);
    if (pressed_sample != sw_raw_) {
        sw_raw_ = pressed_sample;
        sw_last_change_us_ = now_us;
    }

    if ((now_us - sw_last_change_us_) >= ENCODER_SW_DEBOUNCE_US) {
        if (sw_debounced_ != sw_raw_) {
            sw_debounced_ = sw_raw_;
            if (sw_debounced_) {
                sw_pressed_event_ = true;
            }
        }
    }
}

int8_t RotaryEncoder::take_delta() {
    const int8_t d = detent_delta_;
    detent_delta_ = 0;
    return d;
}

bool RotaryEncoder::take_button_press() {
    const bool ev = sw_pressed_event_;
    sw_pressed_event_ = false;
    return ev;
}

} // namespace macropad
