cmake_minimum_required(VERSION 3.2)

project(vat_filler)

set (SRC
    src/main.cpp
    src/LiquidSensor.cpp
)

set (INC
    src/Pins.h
    src/LiquidSensor.h
    src/Config.h
)

set( CMAKE_MESSAGE_LOG_LEVEL "STATUS" )

add_executable(vat_filler ${SRC} ${INC})

target_link_arduino_libraries(vat_filler
#    Adafruit_GFX
#    Adafruit_SSD1306
#    RotaryEncoder
#    Wire
    AccelStepper
    core
)


target_enable_arduino_upload(vat_filler)
