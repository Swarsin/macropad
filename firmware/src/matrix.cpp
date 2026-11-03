#include "matrix.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

namespace macropad {

void KeyMatrix::init() {
    // Diodes point from Column (Anode) to Row (Cathode): COL2ROW.
    // We drive the active Column HIGH (1) while inactive Columns stay LOW (0),
    // and sample Row pins configured as inputs with internal pull-downs.
    for (uint8_t c = 0; c < MATRIX_COLS; ++c) {
        gpio_init(COL_PINS[c]);
        gpio_set_dir(COL_PINS[c], GPIO_OUT);
        gpio_put(COL_PINS[c], 0);
    }

    for (uint8_t r = 0; r < MATRIX_ROWS; ++r) {
        gpio_init(ROW_PINS[r]);
        gpio_set_dir(ROW_PINS[r], GPIO_IN);
        gpio_pull_down(ROW_PINS[r]);
    }
}

bool KeyMatrix::scan(uint64_t now_us) {
    bool any_changed = false;
    uint16_t new_mask = 0;

    for (uint8_t c = 0; c < MATRIX_COLS; ++c) {
        // Assert active column HIGH
        gpio_put(COL_PINS[c], 1);
        sleep_us(SETTLE_DELAY_US);

        for (uint8_t r = 0; r < MATRIX_ROWS; ++r) {
            const bool sample = gpio_get(ROW_PINS[r]);

            rose_state_[r][c] = false;
            fell_state_[r][c] = false;

            // Reset per-key debounce timer whenever the raw contact state changes
            if (sample != raw_state_[r][c]) {
                raw_state_[r][c] = sample;
                last_transition_us_[r][c] = now_us;
            }

            // Commit to debounced state once stable for DEBOUNCE_TIME_US (5 ms)
            if ((now_us - last_transition_us_[r][c]) >= DEBOUNCE_TIME_US) {
                if (debounced_state_[r][c] != raw_state_[r][c]) {
                    debounced_state_[r][c] = raw_state_[r][c];
                    rose_state_[r][c] =  debounced_state_[r][c];
                    fell_state_[r][c] = !debounced_state_[r][c];
                    any_changed = true;
                }
            }

            if (debounced_state_[r][c]) {
                new_mask |= static_cast<uint16_t>(1u << (r * MATRIX_COLS + c));
            }
        }

        // De-assert column back LOW before advancing to next column
        gpio_put(COL_PINS[c], 0);
        sleep_us(SETTLE_DELAY_US);
    }

    debounced_mask_ = new_mask;
    return any_changed;
}

bool KeyMatrix::is_pressed(uint8_t row, uint8_t col) const {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return debounced_state_[row][col];
}

bool KeyMatrix::rose(uint8_t row, uint8_t col) const {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return rose_state_[row][col];
}

bool KeyMatrix::fell(uint8_t row, uint8_t col) const {
    if (row >= MATRIX_ROWS || col >= MATRIX_COLS) return false;
    return fell_state_[row][col];
}

} // namespace macropad
