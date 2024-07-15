import cv2
import numpy as np

def removeSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (180, 180, 180), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)   
  
  image = cv2.inpaint(image, mask, inpaintRadius=3, flags=cv2.INPAINT_TELEA)
  return image

def generateMasks(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_RGB2HLS)

  print("HSL:", imageHSL[240][170])

  green = cv2.inRange(imageHSL, (0, 0, 70), (360, 360, 170))
  red   = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))
  white = cv2.inRange(imageHSL, (0, 100, 0), (100, 255, 15))
  
  combined = cv2.bitwise_or(green, red)
  combined = cv2.bitwise_or(combined, white)

  return green, red, white, combined