import random
import math

def generateRandomPoints(xCenter, yCenter, radius, numberOfPoints):
  points = []

  while(len(points) < numberOfPoints):
    angle = random.uniform(0, 2 * math.pi)

    r = math.sqrt(random.uniform(0, 1)) * radius
    x = int(xCenter + r * math.cos(angle))
    y = int(yCenter + r * math.sin(angle))

    if(x < 0 or y < 0 or x > 479 or y > 379): pass
    else:                                     points.append((x, y))

  return points

def isBlack(pixel, threshold):
  return all(value <= threshold for value in pixel)

def typeCheck(circle, image, numberOfPoints):
  if circle[0] == circle[1] and circle[0] == 0: return -1
  points = generateRandomPoints(circle[0], circle[1], circle[2], numberOfPoints)
  blackCount = 0

  for point in points:
    pixel = image[point[1]][point[0]]
    if isBlack(pixel, 50): blackCount += 1

  if(blackCount > 6): return 1
  else:               return 0