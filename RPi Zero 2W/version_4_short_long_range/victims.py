import cv2
import numpy as np
from imageTransform import *

def validateVictims(circles, green, red):
  approvedCircles = []

  for(x, y, r) in circles:
    if  y > 30 \
    and red[y][x] == 0 \
    and green[y][x] == 0: approvedCircles.append((x, y, r))

  return approvedCircles

def findLongVictims(image, gray, green, red):
  circles = None
  circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, dp=2,   minDist=10, param1=100, param2=45, minRadius=20, maxRadius=200)

  if circles is not None:
    circles = np.round(circles[0, :]).astype("int")
    circles = validateVictims(circles, green, red)

    for (x, y, r) in circles:
      cv2.circle(image, (x, y), r, (0, 255, 0), 1)
      cv2.circle(image, (x, y), 1, (0, 255, 0), 1)
      
  else:
    circles = [(-1, -1, -1)]

  return circles, image

def findAverages(circles):
  xTotal, yTotal = 0, 0
  xAverage, yAverage = 0, 0

  for (x, y, r) in circles:
    xTotal += x
    yTotal += y

  xAverage, yAverage = int(xTotal / len(circles)), int(yTotal / len(circles))

  return (xAverage, yAverage, 0)

def findShortVictims(image, gray, green, red):
  circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, dp=1.5, minDist=100, param1=100, param2=30, minRadius=40, maxRadius=150)

  if circles is not None:
    circles = np.round(circles[0, :]).astype("int")
    circles = validateVictims(circles, green, red)

    for (x, y, r) in circles:
      cv2.circle(image, (x, y), r, (0, 0, 255), 1)
      cv2.circle(image, (x, y), 1, (0, 0, 255), 1)
  
  else:
    circles = [(-1, -1, -1)]

  return circles, image

def matchVictims(image, circles, previousCircles, tolorance):
  approvedCircles = []

  for (x1, y1, r1) in circles:
    for (x2, y2, r2) in previousCircles:
      delta = abs(x1 - x2) * 1 + abs(y1 -y2) * 1 + abs(r1 - r2) * 5

      if delta < tolorance and r1 != - 1:
        approvedCircles.append((x1, y1, r1))
        cv2.circle(image, (x1, y1), r1, (255, 0, 0), 1)
        cv2.circle(image, (x1, y1), 1 , (255, 0, 0), 1)

  if len(approvedCircles) == 0:
    approvedCircles.append((-1, -1, -1))

  return approvedCircles, image