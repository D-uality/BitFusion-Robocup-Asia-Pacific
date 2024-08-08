from config import *
import cv2
import numpy as np

def isGray(pixel, threshold):
  return all(value <= threshold for value in pixel)

def presenceCheck(image):
  x, y = int(REDUCTION_WIDTH / 2), REDUCTION_HEIGHT - 130

  # print(f"    {image[x, y]}")
  cv2.circle(image, (x, y), 1, (255, 255, 255), 1)

  if isGray(image[x, y], 100): return 0
  else:                        return 1

