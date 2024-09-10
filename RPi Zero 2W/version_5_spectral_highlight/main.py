import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import serial

from config import *
from camera import *
from imageTransform import *
from victim import *

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

def convertStringToBytes(message):
  return list(map(ord, message))

try:
  while True:
    timeStart = time.time()

    image = camera.capture_array()
    image = image[CROP_MIN:CROP_MAX][:]

    x, y, r, greenX, redX, victimType = -1, -1, -1, -1, -1, -1

    image, spectralHighlightMask = findSpectralHighlights(image, 51)
    imageHSL, green, red, black = generateMasks(image)

    xLive, yLive = findLiveVictims(spectralHighlightMask, image)
    xDead, yDead = findDeadVictims(black, image)

    data = " " + str(x) + " " + str(y) + " "
    print(f"Sending {data}", end="    ")
    com.write(convertStringToBytes(data))

    cv2.imshow("image", image)
    cv2.imshow("Spectral Highlights", spectralHighlightMask)

    print(f"|    {(1 / (time.time() - timeStart)):.2f} pc/s")

except KeyboardInterrupt:
  print("Exiting program!")