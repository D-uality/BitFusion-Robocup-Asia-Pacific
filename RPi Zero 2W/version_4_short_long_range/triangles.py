from config import *
import cv2
import numpy as np

def findTriangles(image, green, red):
  greenContours, _ = cv2.findContours(green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
  redContours, _    = cv2.findContours(red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  image, greenX = positionContour(image, greenContours)
  image, redX   = positionContour(image, redContours  )

  return image, greenX, redX

def positionContour(image, contours):
  maxArea = 0
  maxContour = None
  minX, maxX, averageX = WIDTH, 0, -1

  for contour in contours:
    area = cv2.contourArea(contour)

    if area > maxArea and area > 12000:
      maxArea = area
      maxContour = contour
  
  if maxContour is not None:
    cv2.drawContours(image, maxContour, -1, (255, 0, 0), 3)

    for point in maxContour:
      minX = min(minX, point[0][0])
      maxX = max(maxX, point[0][0])

    averageX = int((minX + maxX) / 2)

  else:
    averageX = 9999

  return image, averageX