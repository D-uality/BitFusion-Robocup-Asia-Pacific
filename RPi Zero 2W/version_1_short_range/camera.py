import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import sys
import serial
import time
from camera_helper import *
from triangles import *

WIDTH  = 480
HEIGHT = 480

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=0, hflip=0))
camera.configure(Configuration)
camera.start()
# camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

com = serial.Serial('/dev/ttyS0', 115200, timeout=1)

def removeSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (180, 180, 180), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  image = cv2.inpaint(image, mask, inpaintRadius=3, flags=cv2.INPAINT_TELEA)
  return image

def findVictims(image, dp, minDist, param1, param2, minRadius, maxRadius):
  output = image.copy()
  grayImage = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
  circles = cv2.HoughCircles(grayImage, cv2.HOUGH_GRADIENT, dp, minDist, param1=param1, param2=param2, minRadius=minRadius, maxRadius=maxRadius)

  if circles is not None:
    circles = np.round(circles[0, :]).astype("int")
    circles = validateVictims(circles)

    for (x, y, r) in circles:
      cv2.circle(output, (x, y), r, (0, 255, 0), 1)
      cv2.circle(output, (x, y), 1, (0, 0, 255), 1)
  else:
    circles = [(0, 0, 0)]

  return circles, output

def validateVictims(circles):
  approvedCircles = []

  for (x, y, r) in circles:
    if y > 30 and y < 150: approvedCircles.append((x, y, r))

  return approvedCircles

def matchVictims(circles, tolorance, image):
  output = image.copy()
  approvedCircles = []

  for (x1, y1, r1) in circles:
    for (x2, y2, r2) in previousCircles:
      delta = abs(x1 - x2) * 1 + abs(y1 -y2) * 2 + abs(r1 - r2) 
      if delta < tolorance:
        approvedCircles.append((x1, y1, r1))
        cv2.circle(output, (x1, y1), r1, (0, 255, 0), 1)
        cv2.circle(output, (x1, y1), 1 , (0, 0, 255), 1)

  if len(approvedCircles) == 0:
    approvedCircles.append((0, 0, 0))
  
  maxRadiusCircle = max(approvedCircles, key=lambda x: x[2])

  return maxRadiusCircle, output

previousCircles = [(0, 0, 0)]

def convertStringToBytes(message):
  return list(map(ord, message))

def transmitData(circle, greenX, redX, victimType):
  currentTries, maximumTries = 0, 7

  data = "0" + str(circle[0]) + "," + str(circle[1]) + "," + str(circle[2]) + "," + str(greenX) + "," + str(redX) + "," + str(victimType)
  print(data, end="    ")

  while currentTries < maximumTries:
    try:
      com.write(convertStringToBytes(data))
      print("    Data sent successfully!", end="    ")
      return True
    except IOError:
      print("    Data failure! Retrying...")
      time.sleep(0.1)
      currentTries += 1

  exit()

try:

  imageCutoff = 50

  while True:
    start = time.time()

    image = camera.capture_array()
    image = image[imageCutoff:][:]
    highlightlessImage = removeSpectralHighlights(image, 7 )
    circles, circleImage = findVictims(highlightlessImage, dp=1.5, minDist=100, param1=100, param2=30, minRadius=40, maxRadius=150)
    matchingCircle, matchingImage = matchVictims(circles, 100, highlightlessImage)

    imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)
    # green = cv2.inRange(imageHSL, (0, 0, 80), (360, 360, 160))
    green = cv2.inRange(imageHSL, (0, 0, 50), (160, 220, 200))
    # red = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))
    red = cv2.inRange(imageHSL, (0, 30, 40), (180, 360, 360))
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    triangles, greenX, redX = findTriangles(highlightlessImage, green, red)
    victimType = typeCheck(matchingCircle, highlightlessImage, 10)
    transmitData(matchingCircle, greenX, redX, victimType)

    # cv2.imshow("Image", image)
    cv2.imshow("Highlightless", highlightlessImage)
    # cv2.imshow("Circles", circleImage)
    cv2.imshow("Matching", matchingImage)
    # cv2.imshow("Triangles", triangles)
    # cv2.moveWindow("Image", 500, 0)
    # cv2.moveWindow("Highlightless", 500, 480-imageCutoff)
    # cv2.moveWindow("Circles", 980, 0)
    # cv2.moveWindow("Matching", 980, 480-imageCutoff)
    # cv2.moveWindow("Triangles", 1460, 0)

    previousCircles = circles

    print(f"{(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Program stopped!");