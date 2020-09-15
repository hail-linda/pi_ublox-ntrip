
# -*- coding:utf-8 -*-
import serial
import time
import sqlite3

def hexshow(data): 
    hex_data = ''
    hLen = len(data) 

    for i in xrange(hLen):
        hvol = ord(data[i])
        hhex = '%02x' % hvol
        hex_data += hhex+' '

    print 'hexshow:', hex_data 

# 加载串口端口信息
ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.5)    # 57600为了和仪器的波特率匹配
ser.flushInput()                                            # 将serial上的接收数据全部清除


while (True):
    
    lenBuffer = ser.inWaiting()
    count = ser.read(lenBuffer)
    print(count)
    #hexshow(count)
    time.sleep(0.8)     # 程序暂停5秒，等待缓存内存入数据。
    

