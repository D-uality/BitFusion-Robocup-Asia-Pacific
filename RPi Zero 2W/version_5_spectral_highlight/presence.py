from config import *
import cv2
import numpy as np
import random
import math

def generateRandomPoints(xCenter, yCenter, radius, numberOfPoints):
  points = []
  cycles = 0

  while len(points) < numberOfPoints and cycles < 15:
    angle = random.uniform(0, 2 * math.pi)
    r = math.sqrt(random.uniform(0, 1)) * radius
    x = int(xCenter + r * math.cos(angle))
    y = int(yCenter + r * math.sin(angle))

    if x < 0 or y < 0 or x > WIDTH - 1 or y > HEIGHT - 1:
      pass
    else:
      points.append((x, y))
    cycles += 1

  return points

def presenceCheck(image, yellow, black):
  yellowContours, _ = cv2.findContours(yellow, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

  if yellowContours is None:
    return 0

  validContours = []

  for contour in yellowContours:
    for point in contour:
      if point[0][0] < int(WIDTH / 2 + 100) and point[0][0] > int(WIDTH / 2 - 100):
        pass
      else:
        validContours.append(contour)

  if len(validContours) == 0:
    return 0

  maxArea = 0
  maxContour = None
  yMin, yMax = HEIGHT, 0
  yAverage = 0

  for contour in validContours:
    area = cv2.contourArea(contour)

    if area > maxArea:
      maxArea = area
      maxContour = contour

  if maxContour is None:
    return 0

  cv2.drawContours(image, maxContour, -1, (255, 0, 0), 2)

  for point in maxContour:
    y = point[0][1]
    
    yMin = min(yMin, y)
    yMax = max(yMax, y)

  yAverage = int((yMax + yMin) / 2)
  print(yAverage, end="    ")

  if yAverage > 200:
    return 0
    
  else:
    points = generateRandomPoints(150, 25, 20, 25) + generateRandomPoints(330, 25, 20, 25)
    
    blackCount = 0

    for point in points:
      if black[point[1]][point[0]] == 255: blackCount += 1
    
    print(blackCount, end="    ")
    if blackCount > 20:
      return 2
    else:
      return 1