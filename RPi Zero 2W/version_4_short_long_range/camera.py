import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

from imageTransform import *
from victims import *
from dataTransmission import *
from config import *

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
camera.configure(Configuration)
camera.start()
cv2.startWindowThread()

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

    xLowerBound, xUpperBound = int(WIDTH / 2) - int(REDUCTION_WIDTH / 2), int(WIDTH / 2) + int(REDUCTION_WIDTH / 2)
    imageSmall, greenSmall, redSmall, graySmall = image[0:REDUCTION_HEIGHT, xLowerBound:xUpperBound].copy(), \
                                                  green[0:REDUCTION_HEIGHT, xLowerBound:xUpperBound].copy(), \
                                                  red  [0:REDUCTION_HEIGHT, xLowerBound:xUpperBound].copy(), \
                                                  gray [0:REDUCTION_HEIGHT, xLowerBound:xUpperBound].copy()

    if mode == 0:
      longVictims, image = findLongVictims(image, gray, green, red)
      longVictims = findAverages(longVictims)

      print(f"    LONG            {longVictims}", end="")
      transmitData(normaliseX(longVictims[0], 1), longVictims[1], longVictims[2])

      if longVictims[0] != -1:
        mode = 1
        previousShortVictims = [(-1, -1, -1)]

    elif mode == 1:
      longVictims, image = findLongVictims(image, gray, green, red)
      longVictims = findAverages(longVictims)

      shortVictims, imageSmall = findShortVictims(imageSmall, graySmall, greenSmall, redSmall)
      matchingShortVicitms, _ = matchVictims(imageSmall, shortVictims, previousShortVictims, 100)

      print(f"    LONG + SHORT    {longVictims}    {shortVictims}", end="")
      transmitData(normaliseX(longVictims[0], 1), longVictims[1], longVictims[2])

      if longVictims[0] == -1:     mode = 0
      if shortVictims[0][0] != -1: mode = 2

      previousShortVictims = shortVictims

    else:
      shortVictims, imageSmall = findShortVictims(imageSmall, graySmall, greenSmall, redSmall)
      matchingShortVicitms, _ = matchVictims(imageSmall, shortVictims, previousShortVictims, 150)

      print(f"    SHORT           {matchingShortVicitms}", end="")
      transmitData(normaliseX(matchingShortVicitms[0][0], 2), matchingShortVicitms[0][1], matchingShortVicitms[0][2])

      if matchingShortVicitms[0][0] == -1: mode = 0

      previousShortVictims = shortVictims

    # cv2.imshow("image", image)
    # cv2.imshow("imageSmall", imageSmall)
    # cv2.moveWindow("image", 0, 20)

    print(f"    {(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Exiting Program!")