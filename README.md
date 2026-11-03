# macropad
![firmware build](https://github.com/Swarsin/macropad/actions/workflows/firmware.yml/badge.svg)

It's a 3x3 macropad pcb, which I designed in KiCad. The microcontroller I used is a Raspberry Pi Pico, which I had several of laying around. Other parts include an oled display, rotary encoder switch (EC11), and mx-style switches, all of which need to be soldered on.

I also wrote some bare-metal C++ firmware for it and modelled an angled 3d-printed case in FreeCAD. The firmware is in `firmware/`, the case files are in `cad/`, and the old CircuitPython/KMK test script is still in `firmware.py`.

Images (3D):
![better](https://github.com/user-attachments/assets/e6979ab8-ff6f-406b-a97c-df21f9071215)
![back](https://github.com/user-attachments/assets/02019b26-1b8f-4f56-bdc8-36aa4eab387a)

## pins

The 3x3 matrix uses GP0-GP2 for rows and GP3-GP5 for columns. The diodes are COL2ROW, with the anode on the switch/column side and the cathode on the row net:

- rows: `GP0` (pin 1), `GP1` (pin 2), `GP2` (pin 4), inputs with pull-downs
- columns: `GP3` (pin 5), `GP4` (pin 6), `GP5` (pin 7), driven high one at a time
- encoder: `A` on `GP17` (pin 22), `B` on `GP16` (pin 21), push switch on `GP18` (pin 24)
- oled: `SDA` on `GP8` (pin 11), `SCL` on `GP9` (pin 12), address `0x3C`

The encoder and OLED both use internal pull-ups, so the encoder common pins can go straight to GND.

I ran ERC and DRC in KiCad 10; both report 0 errors. The remaining warnings are mostly missing external library paths and silkscreen clipping.

## firmware

The firmware is a superloop built with the Pico C/C++ SDK and TinyUSB. It does not use an RTOS.

- the matrix is scanned once per millisecond. One column is driven high, the code waits 5 us for the diodes and traces to settle, then the row inputs are read.
- every key has its own 5 ms debounce timer. Raw state, debounced state and press/release edges are kept separately, so the main loop never blocks on a key.
- the encoder is decoded with a 16-entry Gray-code table. The push switch has a 10 ms debounce.
- USB is a single HID interface with two report IDs: a 6-key rollover keyboard report and a consumer control report. The encoder sends volume up/down and the push switch sends mute.
- the OLED is optional. If it does not ACK on the I2C bus the firmware keeps running without it.

Build it with the Pico SDK and an ARM toolchain:

```bash
cd firmware
mkdir build
cd build
cmake .. -DPICO_SDK_PATH=/path/to/pico-sdk
cmake --build . -j
```

If `PICO_SDK_PATH` is not set, you can use `-DPICO_SDK_FETCH_FROM_GIT=ON` instead. The build produces `macropad_firmware.uf2`, `.elf` and `.bin` files. The GitHub Actions workflow builds the same target and uploads the UF2 as a workflow artifact. Hold BOOTSEL while plugging the Pico in, then copy the UF2 to the `RPI-RP2` drive.

The keymap is in `firmware/include/config.h`. S1-S9 are mapped to 1-9 by default. Change `KEYMAP` there if you want different HID keycodes.

## enclosure

The case is a two-part print for the 95.25 x 95.885 mm board. It has a 6.5 degree typing angle, a 1.5 mm shelf around the PCB, and a deeper area under the board for the through-hole diodes. The top cover has 14 x 14 mm switch cutouts, a 12.5 mm hole for the encoder and a 28 x 15 mm OLED window. It is held on with four snap-fit hooks in the side walls.

The STL files are in `cad/`. Print the bottom flat on its base and the top upside down on its top face. I used PLA at 0.2 mm layers, 3 walls and 20% infill; PETG should also work. No supports are needed.

Assembly is just dropping the populated PCB into the bottom case, lining the Pico USB connector up with the rear opening, and pressing the top cover on until the hooks click. The switches should line up with the plate cutouts as the top goes down.

The FreeCAD project and STEP export are in `cad/`. To regenerate them, run:

```bash
freecadcmd cad/export_enclosure.py
```

That re-exports `cad/Macropad_Enclosure.step` and both STL files from the included `cad/Macropad_Enclosure.FCStd`.

This project includes the Arch Linux logo, which is a trademark of the Arch Linux Project. The use of this logo does not imply any endorsement or affiliation with Arch Linux. For more details, see the Arch Linux Trademark Policy.
