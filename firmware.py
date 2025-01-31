import board
from kmk.kmk_keyboard import KMKKeyboard
from kmk.scanners import DiodeOrientation
from kmk.keys import KC

keyboard = KMKKeyboard()

# keyboard rows and column pins
keyboard.row_pins = (board.GP0, board.GP1, board.GP2)      # Rows 0-2
keyboard.col_pins = (board.GP3, board.GP4, board.GP5)      # Columns 0-2

# diodes pointing towards rows in schematic (cathode to row)
keyboard.diode_orientation = DiodeOrientation.COL2ROW

keyboard.keymap = [
    [
        KC.N1, KC.N2, KC.N3,    
        KC.N4, KC.N5, KC.N6,    
        KC.N7, KC.N8, KC.N9,    
    ],
]

if __name__ == '__main__':
    print("Initializing keyboard with:")
    print("Row pins (GP0, GP1, GP2):", keyboard.row_pins)
    print("Column pins (GP3, GP4, GP5):", keyboard.col_pins)
    print("Diode orientation: COL2ROW")
    keyboard.debug_enabled = True
    keyboard.go()
