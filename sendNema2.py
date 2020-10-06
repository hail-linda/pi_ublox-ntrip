# -*- coding:utf-8 -*-
import os
import sys
import serial
import time

def printdataRead(dataRead):
    lines = dataRead.splitlines()
    for line in lines:
        #print(line[0:6])
        if(line[0:6]=="$GNGGA"):
            with open("GNGGA","w") as f:
                f.write(line)
                print(line)
            return

def hexshow(data): 
    hex_data = ''
    hLen = len(data)
    if hLen > 4 :
        if('%02x' % ord(data[3])=="3e" and ('%02x' % ord(data[4]))[0]=="d") and \
            '%02x' % ord(data[0])=="d3" and '%02x' % ord(data[1])=="00":
                hex_data += "----1005----"
                with open("/home/pi/R1005","w") as f:
                    f.write(data)
    for i in xrange(hLen):
        if('%02x' % ord(data[i])=="d3" and '%02x' % ord(data[i+1])=="00"):
            hex_data += "\n\n"
        hvol = ord(data[i])
        hhex = '%02x' % hvol
        hex_data += hhex+' '
        

    print 'hexshow:', hex_data
    
def sendFile(file):
    with open(files, "rb") as f:
        NEMA = f.read()+"\r\n"
        #print(NEMA)
                
        lenBuffer = ser.inWaiting()
        dataRead = ser.read(lenBuffer)
        printdataRead(dataRead)
        dataSend = ser.write(NEMA)
        print(files)
        print(hexshow(NEMA))
        #hexshow(count)
        
def sendR1005():
    with open("/home/pi/R1005", "rb") as f:
        NEMA = f.read()+"\r\n"
        #print(NEMA)
                
        lenBuffer = ser.inWaiting()
        dataRead = ser.read(lenBuffer)
        printdataRead(dataRead)
        dataSend = ser.write(NEMA)
        print(files)
        print(hexshow(NEMA))
        #hexshow(count)



# 加载串口端口信息
ser = serial.Serial("/dev/ttyUSB0", 115200
                    
                    )    # 57600为了和仪器的波特率匹配
ser.flushInput()                                            # 将serial上的接收数据全部清除


while (True):
    path = sys.path[0]
    path = "/home/pi"
    numOfFile = 0
    fileList=[]
    for files in os.listdir(path): 
        if(files[-5:]==".nema"):
            fileList.append(files[:-5])
    fileList.sort()
    #print(fileList)
    
    if(len(fileList) >= 2):
        files = fileList[0]+".nema"
        files = "/home/pi/"+files
        sendFile(files)
        #print("filelast",fileList[0][-1])
        if(fileList[0][-1]=="0"):
            sendR1005()
        #print(files) 
        os.remove(files)
    if(len(fileList) < 2):
        time.sleep(0.8)     # 程序
    '''
for files in os.listdir(path): 
    if(files[-5:]==".nema"):
        print(files)
        os.remove(files)
        '''

