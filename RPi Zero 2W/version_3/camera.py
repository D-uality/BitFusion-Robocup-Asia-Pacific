import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import serial
import time

from imageTransform import *
from victims import *

HEIGHT = 1080
WIDTH  = 1920 

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
camera.configure(Configuration)
camera.start()
cv2.startWindowThread()

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

previousCircles = [(0, 0, 0)]

try:
  while True:
    image = camera.capture_array()
    image = image[350:][:]

    image = removeSpectralHighlights(image, 5)
    image, green, red, gray = genearteMasks(image)

    circles, image = findVictims(image, gray, green, red)
    matchingCircles, image = matchVictims(image, circles, previousCircles, 5)

    cv2.imshow("image", image)
    cv2.moveWindow("image", 0, 0)

    input()

    previousCircles = circles
    print(previousCircles)

except KeyboardInterrupt:
  print("Exiting Program!") 