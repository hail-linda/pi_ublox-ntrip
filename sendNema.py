import os
import sys
import serial

try:
	portx="/dev/ttyUSB0"
	bps = 115200
	ser = serial.Serial(portx,bps,timeout=5)


path = sys.path[0]
lastNema = 0;
for files in os.listdir(path): 
	if(files[-5:]==".nema"):
		if(files[:-5]>lastNema):
			lastNema = files[:-5]
		
files = str(lastNema)+".nema"
fileSize = os.path.getsize(files)
with open(files, "rb") as f:
	NMEA = f.read()
	print(NMEA)
	'''
for files in os.listdir(path): 
	if(files[-5:]==".nema"):
		print(files)
		os.remove(files)
		'''

