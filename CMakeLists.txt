cmake_minimum_required(VERSION 3.2)

project(vat_filler)

set (SRC
    src/main.cpp
    src/Menu.cpp
    src/LiquidSensor.cpp
)

set (INC
    src/Menu.h
    src/MenuItem.h
    src/Pins.h
    src/LiquidSensor.h
)

set( CMAKE_MESSAGE_LOG_LEVEL "STATUS" )

add_executable(vat_filler ${SRC} ${INC})

#find_arduino_library(Adafruit_BusIO "/home/thicccatto/Arduino/libraries/Adafruit_BusIO")

target_link_arduino_libraries(vat_filler
    Adafruit_GFX
    Adafruit_SSD1306
    Adafruit_I2CDevice
    RotaryEncoder
    Wire
    core
)


target_enable_arduino_upload(vat_filler)
