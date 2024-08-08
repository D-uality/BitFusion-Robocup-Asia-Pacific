import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import sys
import smbus2 as smbus
import time

WIDTH  = 480
HEIGHT = 480

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.configure(Configuration)
camera.start()
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

def findTriangles(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)

  cv2.circle(image, (240, 50), 1, (0, 255, 0), 2)
  print(imageHSL[240, 50])

  red   = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))
  green = cv2.inRange(imageHSL, (0, 0, 80), (360, 360, 180));

  cv2.imshow("red  ", red  )
  cv2.imshow("green", green)

try:
  while True:
    image = camera.capture_array()
    image = image[100:][:]

    findTriangles(image)

    cv2.imshow("image", image)
except KeyboardInterrupt:
  print("Exiting")