from config import *
import cv2
import numpy as np

def findSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (200, 200, 200), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  return image, mask

def generateMasks(image):
  imageHSL = cv2.cvtColor(image, cv2.COLOR_BGR2HLS)
  green = cv2.inRange(imageHSL, (0, 0, 80), (360, 360, 160))
  red = cv2.inRange(imageHSL, (0, 0, 160), (360, 360, 360))
  gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

  return imageHSL, green, red, gray