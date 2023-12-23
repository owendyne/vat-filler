#!/bin/bash

# arduino-cmake-toolchain requires the library directory to:
# be named after the basename of a contained header
ARDUINO_LIB_DIR=$HOME/Arduino/libraries/

# 1. Adafruit_BusIO
ln -s $ARDUINO_LIB_DIR/Adafruit_BusIO $ARDUINO_LIB_DIR/Adafruit_BusIO_Register
