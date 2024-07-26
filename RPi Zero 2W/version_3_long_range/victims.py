import cv2
import numpy as np

def validateVictims(circles, green, red):
  approvedCircles = []

  for(x, y, r) in circles:
    if  y > 30 \
    and red[y][x] == 0 \
    and green[y][x] == 0: approvedCircles.append((x, y, r))

  return approvedCircles

def findVictims(image, gray, green, red):
  dp = 2
  minDist = 10
  param1 = 100
  param2 = 50
  minRadius = 20
  maxRadius = 200

  circles = cv2.HoughCircles(gray, cv2.HOUGH_GRADIENT, dp, minDist, param1=param1, param2=param2, minRadius=minRadius, maxRadius=maxRadius)

  if circles is not None:
    circles = np.round(circles[0, :]).astype("int")
    circles = validateVictims(circles, green, red)

    for (x, y, r) in circles:
      cv2.circle(image, (x, y), r, (0, 255, 0), 1)
      cv2.circle(image, (x, y), 1, (0, 255, 0), 1)
      
  else:
    circles = [(0, 0, 0)]

  return circles, image

def matchVictims(image, circles, previousCircles, tolorance):
  approvedCircles = []

  for (x1, y1, r1) in circles:
    for (x2, y2, r2) in previousCircles:
      delta = abs(x1 - x2) * 1 + abs(y1 -y2) * 1 + abs(r1 - r2) * 1
      if delta < tolorance:
        approvedCircles.append((x1, y1, r1))
        cv2.circle(image, (x1, y1), r1, (0, 0, 255), 1)
        cv2.circle(image, (x1, y1), 1 , (0, 0, 255), 1)

  if len(approvedCircles) == 0:
    approvedCircles.append((0, 0, 0))

  return approvedCircles, image

def calculateAverage(circles):
  xTotal, yTotal = 0, 0

  for (x, y, r) in circles: 
    xTotal += x
    yTotal += y

  print(f"{xTotal / len(circles):.2f} {yTotal / len(circles):.2f}", end="")

  return xTotal / len(circles), yTotal / len(circles)