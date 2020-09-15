# -*- coding:utf-8 -*-
import os
import sys
import serial
import time

def hexshow(data): 
    hex_data = ''
    hLen = len(data) 

    for i in xrange(hLen):
        if('%02x' % ord(data[i])=="d3" and '%02x' % ord(data[i+1])=="00"):
            hex_data += "\n\n"
        hvol = ord(data[i])
        hhex = '%02x' % hvol
        hex_data += hhex+' '

    print 'hexshow:', hex_data 

# 加载串口端口信息
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5)    # 57600为了和仪器的波特率匹配
ser.flushInput()                                            # 将serial上的接收数据全部清除


while (True):
    path = sys.path[0]
    for files in os.listdir(path): 
        if(files[-5:]==".nema"):
            with open(files, "rb") as f:
                NEMA = f.read()+"\r\n"
                print(NEMA)
                
                lenBuffer = ser.inWaiting()
                dataRead = ser.read(lenBuffer)
                print(dataRead)
                dataSend = ser.write(NEMA)
                #print(files)
                print(hexshow(NEMA))
                #hexshow(count)

            os.remove(files)
    time.sleep(0.2)     # 程序暂停5秒，等待缓存内存入数据。
    '''
for files in os.listdir(path): 
    if(files[-5:]==".nema"):
        print(files)
        os.remove(files)
        '''

