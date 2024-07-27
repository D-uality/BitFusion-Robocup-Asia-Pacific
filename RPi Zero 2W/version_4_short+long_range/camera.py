import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import serial

from imageTransform import *
from victims import *

HEIGHT = 1080
WIDTH  = 1920 

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
camera.configure(Configuration)
camera.start()
cv2.startWindowThread()

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------

previousShortVictims = [(0, 0, 0)]

try:
  while True:
    start = time.time()

    image = camera.capture_array()
    image = image[300:][:]

    image = removeSpectralHighlights(image, 5)
    imageHSL, green, red, gray = generateMasks(image)

    imageSmall, greenSmall, redSmall, graySmall = image[0:480, 720:1200], green[0:480, 720:1200], red[0:480, 720:1200], gray[0:480, 720:1200]

    longVictims, image = findLongVictims(image, gray, green, red)
    longVictims = findAverages(longVictims)
    shortVictims, imageSmall = findShortVictims(imageSmall, graySmall, greenSmall, redSmall)
    matchingShortVicitms = matchVictims(imageSmall, shortVictims, previousShortVictims, 100)

    print(longVictims, shortVictims, end="")

    cv2.imshow("image", image)
    cv2.imshow("imageSmall", imageSmall)
    cv2.moveWindow("image", 0, 0)

    previousShortCircles = shortVictims

    print(f"    {(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Exiting Program!")