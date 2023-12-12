import time
import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation

dataList = []                                           # Create empty list variable for later use

ser = serial.Serial("COM8", 9600)                       # Establish Serial object with COM port and BAUD rate to match Arduino Port/rate

ser.write(b'g')                                     # Transmit the char 'g' to receive the Arduino data point
arduinoData_string = ser.readline().decode('ascii') # Decode receive Arduino data as a formatted string

res = [float(idx) for idx in arduinoData_string.split()]

print(res)
time.sleep(1)                                           # Time delay for Arduino Serial initialization 
                                          # Close Serial connection when plot is closed
ser.close()
