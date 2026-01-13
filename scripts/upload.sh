#!/bin/bash
arduino-cli compile -b arduino:avr:uno
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi

arduino-cli upload -b arduino:avr:uno -p /dev/ttyACM0
