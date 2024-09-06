import cv2
import numpy as np
from picamera2 import Picamera2, Preview
from libcamera import Transform, controls
import time

from config import *
from camera import *
from imageTransform import *
from victim import *

try:
  while True:
    timeStart = time.time()

    image = camera.capture_array()
    image = image[CROP_MIN:CROP_MAX][:]

    x, y, r, greenX, redX, victimType = -1, -1, -1, -1, -1, -1

    image = removeSpectralHighlights(image, 9)
    imageHSL, green, red, gray = generateMasks(image)

    

    print(f"    {(1 / (time.time() - timeStart)):.2f} pc/s")
except KeyboardInterrupt:
  print("Exiting program!")