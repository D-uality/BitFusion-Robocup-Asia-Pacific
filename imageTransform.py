from config import *
import cv2
import numpy as np

def findSpectralHighlights(image, kernalSize):
  mask = cv2.inRange(image, (240, 240, 240), (255, 255, 255))
  
  kernel = np.ones((kernalSize, kernalSize), np.uint8)
  mask = cv2.dilate(mask, kernel, iterations=1)
  
  return image, mask

def generateMasks(image):
  rgb_image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
  green = cv2.inRange(rgb_image, (15, 35, 25), (65, 130, 90))
  red = cv2.inRange(rgb_image, (60, 0, 5), (160, 30, 40))
  # Increased blur
  black = cv2.GaussianBlur(rgb_image, (7, 7), 0)  # Increased kernel size
  black = cv2.inRange(black, (0, 0, 0), (30, 40, 30))

  return rgb_image, green, red, black
