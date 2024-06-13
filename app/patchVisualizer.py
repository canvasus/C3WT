import os
import json
import tkinter as tk
from tkinter.filedialog import askopenfile, askdirectory
from tkinter import *
from tkinter.ttk import *

def readPatch(file):
    content = file.read()
    return content

  
def on_closing():
    root.destroy()

root = tk.Tk()
root.geometry('300x150')
root.title("C3WT Patch Visualizer")
C = Canvas(root, bg="grey",
           height=250, width=300)
osc1 = C.create_rectangle(50, 50, 100, 60,
                                outline = "black", fill = "blue",
                                width = 2)
osc1.
C.pack()

root.protocol("WM_DELETE_WINDOW", on_closing)
#root.after(100, mainTask)
root.mainloop()

