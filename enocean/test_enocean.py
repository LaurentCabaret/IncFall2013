#!/usr/bin/env python
# test_communication_arduino.py
# -----------------------------
# Copyright : (c) 2013, Xavier Lagorce <Xavier.Lagorce@crans.org>
# Licence   : BSD3

import time, serial, sys

# Get serial device
serial_dev = "/dev/ttyUSB0"
if len(sys.argv) > 1:
        serial_dev = sys.argv[1]

ser = serial.Serial(serial_dev, 57600)

last_btn = 0

while True:
        if ser.inWaiting() >= 21:
                pkt = ser.read(21)
                cmd = list("BT1D")
                if (ord(pkt[12]) == 0x20):
                        cmd[3] = "U"
                if (ord(pkt[7]) == 0):
                        cmd[2] = last_btn
                elif (ord(pkt[7]) == 0x50):
                        cmd[2] = '0'
                        last_btn = '0'
                elif (ord(pkt[7]) == 0x70):
                        cmd[2] = '1'
                        last_btn = '1'
                else:
                        cmd[2] = '2'
                        last_btn = '2'
                print "".join(cmd)

#ser.write('R0\r')

ser.close()
