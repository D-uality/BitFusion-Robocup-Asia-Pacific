import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform
import time

WIDTH  = 480
HEIGHT = 480

Camera = Picamera2()
Configuration = Camera.create_preview_configuration(main={"format": "RGB888", "size": (WIDTH, HEIGHT)}, transform = Transform(vflip=1, hflip=1))
Camera.configure(Configuration)
Camera.start()
cv2.startWindowThread()

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

    for (x, y, r) in circles:
      cv2.circle(image, (x, y), r, (0, 255, 0), 2)
      cv2.circle(image, (x, y), 1, (0, 0, 255), 3)

  '''
    If radius is too small, remove
    If yLevel is too small, remove
    If different sized circles over two images, remove
    + Continuous spinning/movement -> changes the image
    Relativity approach.
  '''

  return circles, image

try:
  while True:
    start = time.time()

    image = Camera.capture_array()
    image = image[100:][:]
    greyImage = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    cv2.imshow("Image", image);
    cv2.moveWindow("Image", 700, 50)
    # cv2.imshow("Grey", greyImage)

    # differenceImage, normalImage = normaliseImage(greyImage, 11, 31)
    # cv2.imshow("Difference Image", differenceImage)
    # cv2.imshow("Normalised Image", normalImage)

    circles, circleImage = findVictims(greyImage, 1, 100, 100, 15, 4, 100)

    cv2.imshow("Circles", circleImage)
    cv2.moveWindow("Circles", 700, 500)

    image2 = Camera.capture_array()
    image2 = image2[100:][:]
    greyImage2 = cv2.cvtColor(image2, cv2.COLOR_BGR2GRAY)

    cv2.imshow("Image2", image2);
    cv2.moveWindow("Image2", 1300, 50)

    circles2, circleImage2 = findVictims(greyImage2, 1, 100, 100, 15, 4, 100)

    cv2.imshow("Circles2", circleImage2)
    cv2.moveWindow("Circles2", 1300, 500)

    print(f"{(1 / (time.time() - start)):.2f} fps")

except KeyboardInterrupt:
  print("Program stopped!");