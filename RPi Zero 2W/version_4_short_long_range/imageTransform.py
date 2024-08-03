from config import *
import cv2
import numpy as np

def removeSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (200, 200, 200), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  image = cv2.inpaint(image, mask, inpaintRadius=3, flags=cv2.INPAINT_TELEA)

  return image

def generateMasks(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)
  green = cv2.inRange(imageHSL, (0, 0, 80), (360, 360, 160))
  red = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))
  gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

  return imageHSL, green, red, gray

def noramliseCoordinates(x, imageSize):
  if x == -1: return -1
  else:       return int(x - (WIDTH / 2)) if imageSize == 0 else int(x - (REDUCTION_WIDTH / 2))