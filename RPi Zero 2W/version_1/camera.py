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

try:
  while True:
    start = time.time()

    image = Camera.capture_array()
    image = image[100:][:]
    output = image.copy()
    greyImage = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    cv2.imshow("Image", image);
    # cv2.imshow("Grey", greyImage)

    differenceImage, normalImage = normaliseImage(greyImage, 3, 31)
    
    # cv2.imshow("Difference Image", differenceImage)
    # cv2.imshow("Normalised Image", normalImage)
    
    dp = 1.1                            # Inverse ratio of the accumulator resolution to the image resolution
    minDist = 50                        # Minimum distance between the centers of detected circles
    param1 = 100                        # Upper threshold for the internal Canny edge detector
    param2 = 30                         # Threshold for center detection
    minRadius = 4                       # Minimum radius of the circles to be detected
    maxRadius = 100                      # Maximum radius of the circles to be detected

    circles = cv2.HoughCircles(normalImage, cv2.HOUGH_GRADIENT, dp, minDist, param1=param1, param2=param2, minRadius=minRadius, maxRadius=maxRadius)

    if circles is not None:
      circles = np.round(circles[0, :]).astype("int")
            
      for (x, y, r) in circles:
        cv2.circle(output, (x, y), r, (0, 255, 0), 2)
        cv2.circle(output, (x, y), 1, (0, 0, 255), 3)
        cv2.imshow("Detected Circles", output)
    else:
      print("No circle detected")

    print(f"{(1 / (time.time() - start)):.2f} FPS")

except KeyboardInterrupt:
  print("Program stopped!");