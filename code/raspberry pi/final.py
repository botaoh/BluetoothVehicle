import serial
import sys
import keyboard
import time

# Initialize the serial connection (adjust '/dev/ttyACM0' as needed)
try:
	ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
	ser.flush()  # Clear any existing data in the serial buffer
except serial.SerialException as e:
	print(f"Error opening serial port: {e}")
	sys.exit(1)

print("Control the ball with 'w', 's', 'a', 'd'. Press 'q' to quit.")

try:
	while True:
		command = 'u'
		if keyboard.is_pressed('c'):
			command = 'w'
			ser.write(command.encode())  # Send the comma
		elif keyboard.is_pressed('d'):
			command = 's'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('e'):
			command = 'a'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('f'):
			command = 'd'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('g'):
			command = 'a'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('h'):
			command = 'x'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('i'):
			command = 'y'
			ser.write(command.encode())  # Send the command to the mbed
		elif keyboard.is_pressed('j'):
			command = 'b'
			ser.write(command.encode())  # Send the command to the mbed
		else:
			ser.write(command.encode())  # Send the comma
		print(f"Sent command: {command}")
		for i in range(100000):
			j = 1

finally:
	# Ensure the serial connection is closed properly
	ser.close()

