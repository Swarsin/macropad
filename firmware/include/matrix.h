#ifndef MACROPAD_MATRIX_H
#define MACROPAD_MATRIX_H

#include <cstdint>
#include "config.h"

namespace macropad {

class KeyMatrix {
public:
    KeyMatrix() = default;

    // Configure Column GPIOs as push-pull outputs and Row GPIOs as pull-down inputs
    void init();

    // Perform one full 3x3 matrix scan and run the per-key debounce filter.
    // Returns true if the debounced key state changed on this scan cycle.
    bool scan(uint64_t now_us);

    // Query debounced state for a specific key at (row, col)
    bool is_pressed(uint8_t row, uint8_t col) const;

    // Query rising edge (key just pressed on the latest scan)
    bool rose(uint8_t row, uint8_t col) const;

    // Query falling edge (key just released on the latest scan)
    bool fell(uint8_t row, uint8_t col) const;

    // Return a 9-bit bitmask of currently pressed keys (bit = row * 3 + col)
    uint16_t pressed_mask() const { return debounced_mask_; }

private:
    bool     raw_state_[MATRIX_ROWS][MATRIX_COLS]{};
    bool     debounced_state_[MATRIX_ROWS][MATRIX_COLS]{};
    bool     rose_state_[MATRIX_ROWS][MATRIX_COLS]{};
    bool     fell_state_[MATRIX_ROWS][MATRIX_COLS]{};
    uint64_t last_transition_us_[MATRIX_ROWS][MATRIX_COLS]{};
    uint16_t debounced_mask_ = 0;
};

} // namespace macropad

#endif // MACROPAD_MATRIX_H
