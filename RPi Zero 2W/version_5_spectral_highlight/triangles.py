import cv2
import numpy as np
import random
import math

WIDTH  = 480
HEIGHT = 480

def findTriangles(image, green, red):
    greenContours, _ = cv2.findContours(green, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    redContours, _ = cv2.findContours(red, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    image, greenX = positionContour(image, greenContours)
    image, redX = positionContour(image, redContours)
    greenX = verifyGreen(greenX, 0, green)  # Pass 0 for greenY since it's not used

    return image, greenX, redX

def generateRandomPoints(xCenter, yCenter, radius, numberOfPoints):
    points = []
    cycles = 0

    while len(points) < numberOfPoints and cycles < 15:
        angle = random.uniform(0, 2 * math.pi)
        r = math.sqrt(random.uniform(0, 1)) * radius
        x = int(xCenter + r * math.cos(angle))
        y = int(yCenter + r * math.sin(angle))

        if x < 0 or y < 0 or x > WIDTH - 1 or y > HEIGHT - 1:
            pass
        else:
            points.append((x, y))
        cycles += 1

    return points

def verifyGreen(greenX, greenY, green):
    points = generateRandomPoints(greenX, greenY, 7, 25)

    blackCount = 0
    for point in points:
        pixel = green[point[1]][point[0]]
        if pixel == 0:
            blackCount += 1

    if blackCount < 14:
        return greenX
    else:
        return 9999

def calculateAngle(p1, p2, p3):
    # Calculate vectors
    v1 = np.array([p1[0] - p2[0], p1[1] - p2[1]])
    v2 = np.array([p3[0] - p2[0], p3[1] - p2[1]])

    # Normalize vectors
    v1_norm = np.linalg.norm(v1)
    v2_norm = np.linalg.norm(v2)

    # Handle division by zero
    if v1_norm == 0 or v2_norm == 0:
        return 0

    # Calculate dot product
    dot_product = np.dot(v1, v2)

    # Calculate the angle in radians and then convert to degrees
    angle = np.arccos(dot_product / (v1_norm * v2_norm))
    angle = np.degrees(angle)

    return angle

def positionContour(image, contours):
    maxContour = getMaxAreaContour(contours)

    if maxContour is None:
        return image, 9999

    cv2.drawContours(image, maxContour, -1, (255, 0, 0), 3)

    approx = approximateContour(maxContour)
    cv2.drawContours(image, [approx], -1, (255, 0, 0), 3)

    if len(approx) in [3, 4]:
        points = [approx[i][0] for i in range(len(approx))]
        angles = calculateAngles(points)

        if any(isCloseTo90(angle) for angle in angles):
            return image, getContourCenter(maxContour)[0]
            
    return image, 9999

def getMaxAreaContour(contours, min_area=18000):
    maxArea = 0
    maxContour = None
    for contour in contours:
        area = cv2.contourArea(contour)
        if area > maxArea and area > min_area:
            maxArea = area
            maxContour = contour
    return maxContour

def approximateContour(contour, epsilon_factor=0.04):
    epsilon = epsilon_factor * cv2.arcLength(contour, True)
    return cv2.approxPolyDP(contour, epsilon, True)

def calculateAngles(points):
    angles = []
    for i in range(len(points)):
        p1 = points[i]
        p2 = points[(i + 1) % len(points)]
        p3 = points[(i + 2) % len(points)]
        angle = calculateAngle(p1, p2, p3)
        angles.append(angle)
    return angles

def isCloseTo90(angle, tolerance=20):
    return abs(angle - 90) < tolerance

def getContourCenter(contour):
    minX = min(contour[:, 0, 0])
    maxX = max(contour[:, 0, 0])
    minY = min(contour[:, 0, 1])
    maxY = max(contour[:, 0, 1])
    averageX = int((minX + maxX) / 2)
    averageY = int((minY + maxY) / 2)
    return averageX, averageY