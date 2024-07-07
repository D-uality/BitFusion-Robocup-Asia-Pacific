import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time
import sys
import smbus2 as smbus
import time

ARDUINO_ADDRESS = 0x0b;
WIDTH  = 480
HEIGHT = 480

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
camera.configure(Configuration)
camera.start()
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

bus = smbus.SMBus(1)

def removeSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (200, 200, 200), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  image = cv2.inpaint(image, mask, inpaintRadius=3, flags=cv2.INPAINT_TELEA)
  return image

def normaliseImage(image, dilationFactor, blurFactor):
  '''
  Normalise each image on all three axis, RGB.

  Arguments:
  - image: Numpy matrix that contains the image to be normalised
  - dilationFactor: Dilation kernal size
  - blurFactor: Blur kernal size

  Returns:
  - result: Processed image that is subtracted from the maximum (conserves darker/lighter spots)
  - resultNormalised: Processed image that is also normalised (maximises differences)
  '''

  rgbPlanes = cv2.split(image)
  resultantPlanes = []
  resultantNormalPlanes = []

  for plane in rgbPlanes:
    dilatedImage = cv2.dilate(plane, np.ones((dilationFactor, dilationFactor), np.uint8))
    backgroundImage = cv2.medianBlur(dilatedImage, blurFactor)

    planeDifference = 255 - cv2.absdiff(plane, backgroundImage)
    normalisedPlane = cv2.normalize(planeDifference, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8UC1)
    resultantPlanes.append(planeDifference)
    resultantNormalPlanes.append(normalisedPlane)

  result = cv2.merge(resultantPlanes)
  resultNormal = cv2.merge(resultantNormalPlanes)

  return result, resultNormal

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
    if y > 30: approvedCircles.append((x, y, r))

  return approvedCircles

def matchVictims(circles, tolorance, image):
  output = image.copy()
  approvedCircles = []

  for (x1, y1, r1) in circles:
    for (x2, y2, r2) in previousCircles:
      delta = abs(x1 - x2) * 2 + abs(y1 -y2) * 2 + abs(r1 - r2) 
      if delta < tolorance:
        approvedCircles.append((x1, y1, r1))
        cv2.circle(output, (x1, y1), r1, (0, 255, 0), 1)
        cv2.circle(output, (x1, y1), 1 , (0, 0, 255), 1)

  if len(approvedCircles) == 0:
    approvedCircles.append((0, 0, 0))
  return approvedCircles, output

previousCircles = [(0, 0, 0)]

def convertStringToBytes(message):
  return list(map(ord, message))

def transmitData(circles):
  maxRadius = max(circles, key=lambda x: x[2])
  data = str(maxRadius[0]) + "," + str(maxRadius[1]) + "," + str(maxRadius[2])
  bus.write_i2c_block_data(ARDUINO_ADDRESS, 0, convertStringToBytes(data))
  print(data, end="    ")

  '''
    Program keeps crashing
    - Implement retry logic with try ... except ...
    - Following a number of attemps
    - May be because arduino is busy?
  '''

try:
  while True:
    start = time.time()

    image = camera.capture_array()
    image = image[100:][:]
    highlighlessImage = removeSpectralHighlights(image, 7)
    circles, circleImage = findVictims(highlighlessImage, 1, 100, 100, 20, 30, 100)
    matchingCircles, matchingImage = matchVictims(circles, 25, highlighlessImage)

    transmitData(matchingCircles)

    cv2.imshow("Image", image)
    cv2.imshow("Highlightless", highlighlessImage)
    cv2.imshow("Circles", circleImage)
    cv2.imshow("Matching", matchingImage)
    cv2.moveWindow("Image", 500, 0)
    cv2.moveWindow("Highlightless", 500, 480)
    cv2.moveWindow("Circles", 980, 0)
    cv2.moveWindow("Matching", 980, 480)

    previousCircles = circles

    print(f"{(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Program stopped!");