#!/usr/bin/env python3

import sys
import struct
import serial

# Useage
if len(sys.argv) != 2:
    print("Usage: {} /dev/ttyACMx".format(sys.argv[0]))
    sys.exit(1)

# Message Type Definitions          
MESSAGE_POSITION = 1
  
# Open Serial Port
ser = serial.Serial(sys.argv[1])
print("Listening on", ser.name)

# Fetch & Decode
while True:

    # Read in a Log
    data = ser.read(128)
      
    # Get Message Metadata
    meta_data = struct.unpack('<BI', data[0:5])
    log_type = meta_data[0]
    systick = meta_data[1]
    systick /= 10000.0
    
    
    # Handle Position Packet
    if (log_type == MESSAGE_POSITION):
        payload = data[5:26]
        pos = struct.unpack('<iiiBBhBBBBB', payload)
        print("POSITION PACKET:")
        print("Timestamp = ", systick, " s")
        print("lon = ", (pos[0]/10000000), "degrees")
        print("lat = ", (pos[1]/10000000), "degrees")
        print("height = ", (pos[2]/1000), "m")  
        print("num sat = ", pos[3])
        print("fix type = ", pos[4])
        print("date = ", pos[7], "/", pos[6], "/", pos[5], "  ", pos[8], ":", pos[9], ":", pos[10])
        print('\n\n')
 