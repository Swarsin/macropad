cmake_minimum_required(VERSION 3.13)

# Pull in Raspberry Pi Pico SDK (must be before project())
include(pico_sdk_import.cmake)

project(macropad_firmware C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(macropad_firmware
    src/main.cpp
    src/matrix.cpp
    src/encoder.cpp
    src/oled.cpp
    src/usb_descriptors.cpp
)

target_include_directories(macropad_firmware PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(macropad_firmware PUBLIC
    pico_stdlib
    pico_unique_id
    hardware_gpio
    hardware_i2c
    hardware_timer
    tinyusb_device
    tinyusb_board
)

# Disable stdio UART/USB since TinyUSB manages the USB HID interface directly
pico_enable_stdio_usb(macropad_firmware 0)
pico_enable_stdio_uart(macropad_firmware 0)

# Generate UF2, ELF, BIN, and map outputs for flashing to RP2040
pico_add_extra_outputs(macropad_firmware)
