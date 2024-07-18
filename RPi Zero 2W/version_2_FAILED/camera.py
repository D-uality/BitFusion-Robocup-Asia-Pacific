import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import serial
import time

from imageTransform import *
from victims import *

WIDTH  = 480
HEIGHT = 480

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
camera.configure(Configuration)
camera.start()
cv2.startWindowThread()

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

try:
  while True:
    image = camera.capture_array()
    image = image[100:][:]

    highlightlessImage = removeSpectralHighlights(image, 7)

    green, red, white, combined = generateMasks(highlightlessImage)
    print("RGB:", highlightlessImage[240][170], end="    ")

    cv2.circle(image, (240, 170), 2, (255, 255, 255), 1)

    cv2.imshow("image", image)
    cv2.imshow("highlightlessImage", highlightlessImage)

    cv2.imshow("red", red)
    cv2.imshow("green", green)
    cv2.imshow("white", white)
    cv2.imshow("combined", combined)

    cv2.moveWindow("image", 500, 0)
    cv2.moveWindow("highlightlessImage", 500, 380)
    cv2.moveWindow("red", 500+480, 0)
    cv2.moveWindow("green", 500+480, 380)
    cv2.moveWindow("white", 500+480, 760)
    cv2.moveWindow("combined", 500+480+480, 0)

except KeyboardInterrupt:
  print("Exiting Program")