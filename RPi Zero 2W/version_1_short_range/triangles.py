import cv2
import numpy as np
import random
import math

WIDTH  = 480
HEIGHT = 480

def findTriangles(image, green, red):
  greenContours, _ = cv2.findContours(green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
  redContours, _    = cv2.findContours(red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  image, greenX, greenY = positionContour(image, greenContours)
  image, redX, _        = positionContour(image, redContours  )
  greenX = verifyGreen(greenX, greenY, green)
  
  return image, greenX, redX

def generateRandomPoints(xCenter, yCenter, radius, numberOfPoints):
  points = []
  cycles = 0

  while( len(points) < numberOfPoints and cycles < 15):
    angle = random.uniform(0, 2 * math.pi)

    r = math.sqrt(random.uniform(0, 1)) * radius
    x = int(xCenter + r * math.cos(angle))
    y = int(yCenter + r * math.sin(angle))

    if x < 0 or y < 0 or x > WIDTH - 1 or y > HEIGHT - 1: pass
    else:                                                 points.append((x, y))

    cycles += 1

  return points

def verifyGreen(greenX, greenY, green):
  points = generateRandomPoints(greenX, greenY, 7, 25)

  blackCount = 0
  for point in points:
    pixel = green[point[1]][point[0]]
    if pixel == 0: blackCount += 1

  if blackCount < 14: return greenX
  else:              return 9999

def positionContour(image, contours):
  maxArea = 0
  maxContour = None
  minX, maxX, averageX = WIDTH , 0, -1
  minY, maxY, averageY = HEIGHT, 0, -1

  for contour in contours:
    area = cv2.contourArea(contour)

    if area > maxArea and area > 18000:
      maxArea = area
      maxContour = contour
  
  if maxContour is not None:
    cv2.drawContours(image, maxContour, -1, (255, 0, 0), 3)

    for point in maxContour:
      minX = min(minX, point[0][0])
      maxX = max(maxX, point[0][0])
      minY = min(minY, point[0][1])
      maxY = max(maxY, point[0][1])

    averageX = int((minX + maxX) / 2)
    averageY = int((minY + maxY) / 2)

  else:
    averageX = 9999
    averageY = 9999

  return image, averageX, averageY