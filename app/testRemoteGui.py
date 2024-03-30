from graphics import *
import serial

def click1(event):
    print(event)

ser = serial.Serial(port='COM5')
seq = []

win = GraphWin("MAIN", 800, 480)

while True:
    for c in ser.read():
        seq.append(c)
        if chr(c) == '\n':
            if (seq[0] == 255):
                print(seq)
            if (seq[0] == 254):
                joined_seq = ''.join(chr(v) for v in seq[1:-1])
                print(joined_seq)
            seq = []
            break
ser.close()

#win.bind('<Button-1>', click1)  # function's name without `()`
