import serial
import time

ser = serial.Serial('/dev/ttyACM1')  # open serial port


while 1 :

	ser.write(b'a')

	x = ser.readline()


	time.sleep(1)


	print(x)

