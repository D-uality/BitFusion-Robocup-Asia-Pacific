import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

from config import *

# def validateVictims(circles, green, red):
#   approvedCircles = []
#   draw = True

#   for(x, y, r) in circles:
#     if  y > 30  and y < HEIGHT / 2 \
#     and y != HEIGHT and y != 0 \
#     and x != WIDTH  and y != 0 \
#     and red[y][x] == 0 \
#     and green[y][x] == 0: approvedCircles.append((x, y, r))

#   if len(approvedCircles) == 0: 
#     approvedCircles.append((-1, -1, -1))
#     draw = False

#   return approvedCircles, draw

# def findVictims(image, gray, green, red):
#   output = image.copy()
#   circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, dp=1.5, minDist=100, param1=100, param2=30, minRadius=40, maxRadius=150)

#   if circles is not None:
#     circles = np.round(circles[0, :]).astype("int")
#     circles, draw = validateVictims(circles, green, red)

#     if draw:
#       for (x, y, r) in circles:
#         cv2.circle(output, (x, y), r, (0, 255, 0), 1)
#         cv2.circle(output, (x, y), 1, (0, 0, 255), 1)

#   else:
#     circles = [(-1, -1, -1)]

#   print(circles, end="    ")

#   return circles, output

def findVictims(spectralHighlightMask, image):
  contours, _ = cv2.findContours(spectralHighlightMask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  xMin, xMax, xAverage = WIDTH , 0, -1
  yMin, yMax, yAverage = HEIGHT, 0, -1

  if contours:
    for contour in contours:
      for point in contour:
        if point[0][1] > 10:
          yMin = min(yMin, point[0][1])
          yMax = max(yMax, point[0][1])
          xMin = min(xMin, point[0][0])
          xMax = max(xMax, point[0][0])
        

      xAverage = int((xMin + xMax) / 2)
      yAverage = int((yMin + yMax) / 2)

  cv2.drawContours(image, contours, -1, (0, 255, 0), 3)
  cv2.circle(image, (xAverage, yAverage), 1, (255, 0, 0), 2)

  return xAverage, yAverage