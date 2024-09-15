import cv2 
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

from config import *

def findLiveVictims(spectralHighlightMask, image):
  contours, _ = cv2.findContours(spectralHighlightMask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  xMin, xMax, xAverage = WIDTH, 0, 0
  yMin, yMax, yAverage = HEIGHT, 0, 0

  if not contours:
    return xAverage, yAverage

  xSearch, contourIndex = WIDTH, 0

  for i in range(0, len(contours)):
    for point in contours[i]:
      if point[0][0] < xSearch:
        xSearch = point[0][0]
        contourIndex = i

  contour = contours[contourIndex]

  for point in contour:
    if point[0][1] < HEIGHT / 2:
      yMin = min(yMin, point[0][1])
      yMax = max(yMax, point[0][1])
      xMin = min(xMin, point[0][0])
      xMax = max(xMax, point[0][0])

  xAverage = int((xMin + xMax) / 2)
  yAverage = int((yMin + yMax) / 2)

  cv2.drawContours(image, contours, -1, (0, 255, 0), 3)
  cv2.circle(image, (xAverage, yAverage), 1, (255, 0, 0), 2)

  return xAverage, yAverage

def validateContours(contours, yThreshold, minArea):
  approvedContours = []

  for contour in contours:
    yCheck = True

    for point in contour:
      if point[0][1] < yThreshold:
        yCheck = False

    area = cv2.contourArea(contour)
    
    if yCheck and area > minArea:
      approvedContours.append(contour)

  return approvedContours

def isCircle(contour, circularity_threshold=0.7):
  perimeter = cv2.arcLength(contour, True)
  area = cv2.contourArea(contour)

  if perimeter == 0:
    return False

  circularity = (4 * np.pi * area) / (perimeter ** 2)
  return circularity > circularity_threshold

def findDeadVictims(black, image):
  contours, _ = cv2.findContours(black, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  xMin, xMax, xAverage = WIDTH, 0, 0
  yMin, yMax, yAverage = HEIGHT, 0, 0

  contours = validateContours(contours, 10, 1000)

  if contours is None:
    return xAverage, yAverage
  
  for contour in contours:
    if not isCircle(contour):
      continue
    
    print(f"circle true", end="    ")

    for point in contour:
      if point[0][1] < HEIGHT / 2:
        yMin = min(yMin, point[0][1])
        yMax = max(yMax, point[0][1])
        xMin = min(xMin, point[0][0])
        xMax = max(xMax, point[0][0])

    xAverage = int((xMin + xMax) / 2)
    yAverage = int((yMin + yMax) / 2)

    cv2.drawContours(image, contour, -1, (0, 255, 0), 3)
    cv2.circle(image, (xAverage, yAverage), 1, (255, 0, 0), 2)

  return xAverage, yAverage
