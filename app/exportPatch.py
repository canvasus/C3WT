import serial
import json
import tkinter as tk

class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s

    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)

ser = serial.Serial('COM13')
while True:
    rl = ReadLine(ser)
    output = rl.readline()
    print(output)
    json_data_1 = json.loads(output.decode('utf-8'))
    print(json_data_1)
    fileName = "c:/tmp/" + json_data_1['name'].strip() + ".txt"

    f = open(fileName, 'w')
    print(json_data_1, file=f) 
    f.close()

