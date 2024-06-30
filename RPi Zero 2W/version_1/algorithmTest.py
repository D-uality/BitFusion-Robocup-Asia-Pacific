import cv2
import numpy as np
import os
import glob

def getImages(folderPath):
  '''
  Get all JPG images from a folder. Used previously for AI training, except processing times for the TF
  Lite model that was produced was unusable. Hence, using it as testing data for developing an algorithm
  centred around the Hough Circle Transform.

  Arguments:
  - folderPath: A raw string, containing the path to the folder that contains the testing images

  Returns:
  - images: An array of raw strings that contains the path to each individual image.
  '''
  images = glob.glob(os.path.join(folderPath, '*.jpg'))
  return images

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

def main():
  folderPath = r'C:\Users\frede\OneDrive\Documents\Images'
  images = getImages(folderPath)
    
  for imagePath in images:
    image = cv2.imread(imagePath)
    grayImage = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    cv2.imshow("Image", image)
    cv2.imshow("Gray Image", grayImage)
        
    differenceImage, normalImage = normaliseImage(grayImage, 3, 21)
    output = image.copy()

    cv2.imshow("Difference Image", differenceImage)
    cv2.imshow("Normalised Image", normalImage)

    dp = 1.1                            # Inverse ratio of the accumulator resolution to the image resolution
    minDist = 50                        # Minimum distance between the centers of detected circles
    param1 = 100                        # Upper threshold for the internal Canny edge detector
    param2 = 30                         # Threshold for center detection
    minRadius = 4                       # Minimum radius of the circles to be detected
    maxRadius = 50                      # Maximum radius of the circles to be detected

    circles = cv2.HoughCircles(normalImage, cv2.HOUGH_GRADIENT, dp, minDist, param1=param1, param2=param2, minRadius=minRadius, maxRadius=maxRadius)

    if circles is not None:
      circles = np.round(circles[0, :]).astype("int")
            
      for (x, y, r) in circles:
        cv2.circle(output, (x, y), r, (0, 255, 0), 2)
        cv2.circle(output, (x, y), 1, (0, 0, 255), 3)
                
        cv2.imshow("Detected Circles", output)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
    else:
      print("No circle detected")

    cv2.waitKey(0)
    cv2.destroyAllWindows()

main()