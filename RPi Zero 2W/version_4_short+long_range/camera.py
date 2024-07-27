import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import serial

from imageTransform import *
from victims import *

HEIGHT = 1080
WIDTH  = 1900 

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
camera.configure(Configuration)
camera.start()
cv2.startWindowThread()

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------

previousShortVictims = [(-1, -1, -1)]
mode = 0

try:
  while True:
    start = time.time()
    print(f"{mode}", end="")
    
    image = camera.capture_array()
    image = image[300:][:]

    image = removeSpectralHighlights(image, 5)
    imageHSL, green, red, gray = generateMasks(image)

    imageSmall, greenSmall, redSmall, graySmall = image[0:480, 720:1200].copy(), green[0:480, 720:1200].copy(), red[0:480, 720:1200].copy(), gray[0:480, 720:1200].copy()

    if mode == 0:
      longVictims, image = findLongVictims(image, gray, green, red)
      longVictims = findAverages(longVictims)

      print(f"    LONG            {longVictims}", end="")

      if longVictims[0] != -1:
        mode = 1
        previousShortVictims = [(-1, -1, -1)]

    elif mode == 1:
      longVictims, image = findLongVictims(image, gray, green, red)
      longVictims = findAverages(longVictims)

      shortVictims, imageSmall = findShortVictims(imageSmall, graySmall, greenSmall, redSmall)
      matchingShortVicitms, _ = matchVictims(imageSmall, shortVictims, previousShortVictims, 100)

      if longVictims[0] == -1:             mode = 0
      if shortVictims[0][0] != -1: mode = 2

      print(f"    LONG + SHORT    {longVictims}    {shortVictims}", end="")

      previousShortVictims = shortVictims

    else:
      shortVictims, imageSmall = findShortVictims(imageSmall, graySmall, greenSmall, redSmall)
      matchingShortVicitms, _ = matchVictims(imageSmall, shortVictims, previousShortVictims, 150)

      print(f"    SHORT           {shortVictims}", end="")

      if matchingShortVicitms[0][0] == -1: mode = 0

      previousShortVictims = shortVictims

    # cv2.imshow("image", image)
    # cv2.imshow("imageSmall", imageSmall)
    # cv2.moveWindow("image", 0, 0)

    print(f"    {(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Exiting Program!")