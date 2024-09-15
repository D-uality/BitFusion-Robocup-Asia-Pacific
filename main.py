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
from triangles import *

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

def convertStringToBytes(message):
  return list(map(ord, message))

try:
  while True:
    timeStart = time.time()

    image = camera.capture_array()
    imageClean = image.copy()
    image = image[CROP_MIN:CROP_MAX][:]

    x, y, r, greenX, redX, victimType = 0, 0, 0, 0, 0, 0

    image, spectralHighlightMask = findSpectralHighlights(image, 51)
    rgb_image, green, red, black = generateMasks(image)

    image, greenX, redX = findTriangles(image, green, red)

    xLive, yLive = findLiveVictims(spectralHighlightMask, image)
    xDead, yDead = findDeadVictims(black, image)

    data = "0" + str(xLive) + " " + str(yLive) + " " + str(xDead) + " " + str(yDead) + " " + str(greenX) + " " + str(redX) 
    print(f"Sending {data}", end="    ")
    com.write(convertStringToBytes(data))

    if SSH:
      cv2.imshow("image", image)
      cv2.imshow("black", black)
      cv2.imshow("green", green)
      cv2.imshow("red", red)
      
    print(f"|    {(1 / (time.time() - timeStart)):.2f} pc/s")

except KeyboardInterrupt:
  print("Exiting program!")
