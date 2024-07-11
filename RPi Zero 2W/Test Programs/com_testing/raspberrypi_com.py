import serial
import time

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

try:
  while True:
    print("Sending data!")
    com.write("hello arduino!".encode())

    time.sleep(1)    

except KeyboardInterrupt:
  print("Program Stopped!")