import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

WIDTH  = 480
HEIGHT = 480

camera = Picamera2()
Configuration = camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
camera.configure(Configuration)
camera.start()
camera.set_controls({"AfMode":controls.AfModeEnum.Continuous})
cv2.startWindowThread()

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
  circles = cv2.HoughCircles(image, cv2.HOUGH_GRADIENT, dp, minDist, param1=param1, param2=param2, minRadius=minRadius, maxRadius=maxRadius)

  if circles is not None:
    circles = np.round(circles[0, :]).astype("int")
    circles = validateVictims(circles)

    for (x, y, r) in circles:
      cv2.circle(image, (x, y), r, (0, 255, 0), 1)
      cv2.circle(image, (x, y), 1, (0, 0, 255), 1)
  else:
    circles = [(0, 0, 0)]

  print(circles, end="    ")
  return circles, image

previousCircles = []

def validateVictims(circles):
  approvedCircles = []

  for (x, y, r) in circles:
    if y > 30: approvedCircles.append((x, y, r))

  return approvedCircles

try:
  while True:
    start = time.time()

    image = camera.capture_array()
    highlighlessImage = removeSpectralHighlights(image, 7)
    grayImage = cv2.cvtColor(highlighlessImage, cv2.COLOR_RGB2GRAY)
    circles, _ = findVictims(grayImage, 1, 200, 100, 20, 30, 100)

    cv2.imshow("Image", image)
    cv2.imshow("Highlightless", highlighlessImage)
    cv2.imshow("Gray", grayImage)
    cv2.moveWindow("Image", 500, 0)
    cv2.moveWindow("Highlightless", 500, 480)
    cv2.moveWindow("Gray", 980, 0)

    previousCircles = circles

    print(f"{(1/(time.time()-start)):.2f} pc/s")

except KeyboardInterrupt:
  print("Program stopped!");