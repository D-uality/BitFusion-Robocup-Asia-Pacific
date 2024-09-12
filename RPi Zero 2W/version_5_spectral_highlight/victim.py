import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

from config import *

def findLiveVictims(spectralHighlightMask, image):
  contours, _ = cv2.findContours(spectralHighlightMask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  xMin, xMax, xAverage = WIDTH , 0, -1
  yMin, yMax, yAverage = HEIGHT, 0, -1

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

def getMaxAreaContour(contours, min_area=1000):
  maxArea = 0
  maxContour = None

  for contour in contours:
    area = cv2.contourArea(contour)
    if area > maxArea and area > min_area:
      maxArea = area
      maxContour = contour
  return maxContour

def findDeadVictims(black, image):

  cv2.imshow("black", black)
  contours, _ = cv2.findContours(black, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  xMin, xMax, xAverage = WIDTH , 0, -1
  yMin, yMax, yAverage = HEIGHT, 0, -1

  contour = getMaxAreaContour(contours)

  if contour is None:
    return xAverage, yAverage

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
