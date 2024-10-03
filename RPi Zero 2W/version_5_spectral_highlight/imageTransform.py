from config import *
import cv2
import numpy as np

def findSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (200, 200, 200), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  return image, mask

def generateMasks(image):
  rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
  blurred_image = cv2.GaussianBlur(rgb_image, (15, 15), 0) 
  # far_range_green = cv2.inRange(blurred_image, (5, 25, 15), (40, 80, 50))
  # close_range_green = cv2.inRange(blurred_image, (40, 80, 50), (100, 180, 110))
  # green = cv2.bitwise_or(close_range_green, far_range_green)
  green = cv2.inRange(blurred_image, (10, 40, 20), (50, 120, 80))
  red = cv2.inRange(blurred_image, (50, 0, 0), (255, 50, 50))
  black = cv2.inRange(blurred_image, (0, 0, 0), (40, 40, 40))
  yellow = cv2.inRange(blurred_image, (65, 55, 0), (200, 140, 30))
  
  return blurred_image, green, red, black, yellow