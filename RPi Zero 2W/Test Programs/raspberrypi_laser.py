import time
import board
import busio
import adafruit_vl53l0x

i2c = busio.I2C(board.SCL, board.SDA)
sensor_addresses = [0x08, 0x09, 0x0a]

sensors = []
for address in sensor_addresses:
  sensor = adafruit_vl53l0x.VL53L0X(i2c, address=address)
  sensors.append(sensor)

while True:
  for i, sensor in enumerate(sensors):
    distance = sensor.range
    print(f"Sensor {i+1} (Address: {sensor_addresses[i]}) distance: {distance} mm", end="    ")
  
  print()

'''
  Hence, the need to create a distance vs radius cutout, graphing using desmos

  nvm, seems to be requiring different version of numpy, conflicting with camera and opencv ...
'''