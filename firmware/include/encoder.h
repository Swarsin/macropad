#ifndef MACROPAD_ENCODER_H
#define MACROPAD_ENCODER_H

#include <cstdint>

namespace macropad {

class RotaryEncoder {
public:
    RotaryEncoder() = default;

    // Initialize quadrature (A/B) and switch (SW) GPIO pins with internal pull-ups
    void init();

    // Poll quadrature state machine and debounce push switch
    void update(uint64_t now_us);

    // Consume accumulated detents (+1 per CW step, -1 per CCW step)
    int8_t take_delta();

    // Return true once per debounced switch press event
    bool take_button_press();

    // Return current debounced switch state
    bool is_button_pressed() const { return sw_debounced_; }

private:
    uint8_t  ab_state_            = 0x03;
    int8_t   quarter_steps_       = 0;
    int8_t   detent_delta_        = 0;
    bool     sw_raw_              = false;
    bool     sw_debounced_        = false;
    bool     sw_pressed_event_    = false;
    uint64_t sw_last_change_us_   = 0;
};

} // namespace macropad

#endif // MACROPAD_ENCODER_H
