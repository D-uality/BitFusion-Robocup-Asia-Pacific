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
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
camera.configure(Configuration)
camera.start()
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

def findTriangles(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)

  cv2.circle(image, (240, 240), 1, (0, 255, 0), 2)
  print(imageHSL[240][240], end="    ")
  cv2.imshow("hsl", imageHSL)

  green = cv2.inRange(imageHSL, (0, 0, 90), (360, 360, 150))
  contours, _ = cv2.findContours(green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  maxArea = 0
  largestContour = None

  for contour in contours:
    area = cv2.contourArea(contour)
    if area > maxArea and area > 3000:
      maxArea = area
      largestContour = contour

  if largestContour is not None:
    cv2.drawContours(image, largestContour, -1, (255, 0, 0), 2)

    xTotal, yTotal, pointTotal = 0, 0, 0

    for point in largestContour:
      xTotal += point[0][0]
      yTotal += point[0][1]
      pointTotal += 1

    print(f"{xTotal/pointTotal:.2f}    {yTotal/pointTotal:.2f}")

  cv2.imshow("green", green)

try:
  while True:
    image = camera.capture_array()
    image = image[100:][:]

    findTriangles(image)

    cv2.imshow("image", image)
except KeyboardInterrupt:
  print("Exiting")