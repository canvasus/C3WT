import os
import serial
import serial.tools.list_ports
import json
import tkinter as tk
from tkinter.filedialog import askopenfile, askdirectory

def findPort():
    portName = ""
    global teensyOnline
    _teensyOnline = False
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if ( (p.pid == 1161) and (p.vid == 5824)):
            portName = p.device
            if (not teensyOnline):
                _teensyOnline = connectTeensy(portName)
            break
    teensyOnline = _teensyOnline
    global teensyPort
    teensyPort = portName

def connectTeensy(port):
    global ser
    try:
        ser = serial.Serial(port, timeout=0)
        return True
    except:
        return False

def updatePortIndicator(indicator):
    global teensyPort
    if (teensyPort != ""):
        portString = "Teensy 4.x on port: " + teensyPort
        indicator.config(text = portString, bg="lightgreen")
    else:
        portString = "Teensy 4.x not found"
        indicator.config(text = portString, bg="red")

def uploadPatch(file):
    if file is not None:
        content = file.read()
        #print(content)
        try:
            global ser
            ser.write(content.encode())
            messageBox.config(text = "Patch transferred")
        except:
            messageBox.config(text = "Patch transfer failed")
            print("ERROR patch not transfered")

def uploadPack():
    patchId = 0
    directory = askdirectory()
    #print(directory)
    for filename in os.listdir(directory):
        f = os.path.join(directory, filename)
        # checking if it is a file
        if os.path.isfile(f):
            print(f)
            file = open(f, "r")
            uploadPatch(file)
            requestStore(patchId)
            patchId += 1

def uploadSingle():
    file = askopenfile(mode ='r', filetypes =[('Python Files', '*.txt')])
    uploadPatch(file)


def listenTask():
    global ser
    output = ser.readline()
    if(len(output) > 0):
        try:
            json_data_1 = json.loads(output.decode('utf-8'))
            fileName = json_data_1['name'].strip() + ".txt"
            f = open(fileName, 'w')
            print(json_data_1, file=f) 
            f.close()
            message = "Patch received: " + json_data_1['name'].strip()
            messageBox.config(text = message)
        except ValueError:
            messageBox.config(text = "T: " + output.strip())
            #print("decode/store patch failed")
            pass

def requestDump():
    global ser
    try:
        string = "DUMP\n"
        ser.write(string.encode())
    except:
        pass

def requestStore(patchId):
    global ser
    try:
        string = "STORE{_patchId:03d}\n".format(_patchId = patchId)
        ser.write(string.encode())
    except:
        pass

def mainTask():
    findPort()
    updatePortIndicator(portIndicator)
    try:
        listenTask()
    except:
        pass
    root.after(20, mainTask)  # reschedule event in 20ms
    
def on_closing():
    global ser
    root.destroy()
    try:
        ser.close()
    except:
        pass

teensyOnline = False
teensyPort = ""
ser = serial

root = tk.Tk()
root.geometry('300x150')
root.title("C3WT Patch Manager")

portIndicator = tk.Label(root,
                 height = 1,
                 width = 42,
                 bg="lightblue",
                 text="")
portIndicator.place(x=0,y=0)

button_upload_single = tk.Button(root, text="Upload single..."
                   ,command = lambda:uploadSingle()
                   ,height= 2
                   ,width = 16)

button_upload_single.place(x=20,y=30)

button_upload_pack = tk.Button(root, text="Upload pack..."
                   ,command = lambda:uploadPack()
                   ,height= 2
                   ,width = 16)

button_upload_pack.place(x=160,y=30)

button_dump = tk.Button(root, text="Request all patches"
                   ,command = lambda:requestDump()
                   ,height= 2
                   ,width = 36)

button_dump.place(x=20,y=75)


messageBox = tk.Label(root,
                 height = 1,
                 width = 42,
                 bg="lightgrey",
                 text="")
messageBox.place(x=0,y=125)

root.protocol("WM_DELETE_WINDOW", on_closing)
root.after(100, mainTask)
root.mainloop()

