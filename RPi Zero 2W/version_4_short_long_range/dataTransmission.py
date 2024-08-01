from config import *
import serial

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

def convertStringToBytes(message):
  return list(map(ord, message))

def normaliseX(x, mode):
  if   mode == 1: return x - int(WIDTH / 2)
  elif mode == 2: return x - int(REDUCTION_WIDTH / 2)

def transmitData(x, y, r):
  currentTries, maximumTries = 0, 7

  data = str(x) + "," + str(y) + "," + str(r)
  print(f"    {data}", end="")

  while currentTries < maximumTries:
    try:
      com.write(convertStringToBytes(data))
      print("    Data sent successfully!", end="    ")
      return True
    except IOError:
      print("    Data failure! Retrying...")
      time.sleep(0.1)
      currentTries += 1

  exit()