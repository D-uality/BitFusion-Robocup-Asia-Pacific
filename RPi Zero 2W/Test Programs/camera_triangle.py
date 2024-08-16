import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import sys
import smbus2 as smbus
import time

HEIGHT = 800
WIDTH  = 1000

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.configure(Configuration)
camera.start()
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

def findTriangles(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)

  # cv2.circle(image, (240, 50), 1, (0, 255, 0), 2)
  # print(imageHSL[240, 50])

  green = cv2.inRange(imageHSL, (0, 0, 60), (360, 360, 180));
  red   = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))

  greenContours, hierarchy = cv2.findContours(green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
  # redContours, _    = cv2.findContours(red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  image, greenX = positionContour(image, greenContours)
  # image, redX   = positionContour(image, redContours  )

  return image, green, red

def positionContour(image, contours):
  maxArea = 0
  maxContour = None
  minX, maxX, averageX = WIDTH, 0, -1

  for contour in contours:
    area = cv2.contourArea(contour)

    if area > maxArea and area > 18000:
      maxArea = area
      maxContour = contour
  
  print(f"    {maxArea}", end="")

  if maxContour is not None:
    cv2.drawContours(image, maxContour, -1, (255, 0, 0), 3)

    for point in maxContour:
      minX = min(minX, point[0][0])
      maxX = max(maxX, point[0][0])

    averageX = int((minX + maxX) / 2)

  else:
    averageX = 9999

  return image, averageX

try:
  while True:
    image = camera.capture_array()
    image = image[100:][:]

    image, green, red = findTriangles(image)

    cv2.imshow("image", image)
    cv2.imshow("green", green)
    cv2.imshow("red  ", red  )

    print()

except KeyboardInterrupt:
  print("Exiting")