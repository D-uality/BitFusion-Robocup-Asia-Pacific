import sys
import smbus2 as smbus
import time

ARDUINO_ADDRESS = 11;
bus = smbus.SMBus(1)

def convertStringToBytes(message):
    byteMessage = []

    for character in message:
        byteMessage.append(ord(character))
    
    return byteMessage

def main():
    input("Program Started! \nWaiting for key press ...")

    while True:
        print("Sending Data: Hello Arduino!");
        bus.write_i2c_block_data(ARDUINO_ADDRESS, 0, convertStringToBytes("Hello Arduino!"))

        time.sleep(2);

main()