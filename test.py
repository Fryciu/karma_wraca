#!/usr/bin/env python3
import serial
import time

# Adjust this to your serial port (e.g. "COM3" on Windows, "/dev/ttyUSB0" or "/dev/ttyACM0" on Linux)
PORT = "/dev/ttyACM0"
BAUD = 9600

# Open the serial port
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # Wait for the device to reset, often needed for Arduino-type boards

def request(req : str, wait = True):
    ser.write((req.encode() + b'\n'))
    if not wait:
        return
    while True:
        response = ser.readline().decode(errors='replace').strip()
        # if doesn't start with // - return
        if not response.startswith("//"):
            return response

print(request("PING", True))

print("Data sent.")
ser.close()

