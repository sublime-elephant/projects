import math
from tkinter import *

def generatefunction():
    xys =[]
    xys = [(y,((y/20)**2)) for y in range(500)]
    return xys

def recurse(x0,y0,x1,y1,n):
    if n == 0:
        return
    else:
        line1 = canvas1.create_line(x0, y0, x1, y1)



def drawline(x0,y0,x1,y1):
    # center is 615, 335
    #format is x0 y0 x1 y1
    # line1 = canvas1.create_line(615+x0, 335-y0, 615+x1, 335-y1)
    line2 = canvas1.create_line(615+(2*x0), 335+(2*y0), 615-(2*x1), 335-(2*y1), smooth = 1, fill='blue')
    Tk.after(master, 20 ,master.update())


def flipflag():
    global flag
    if flag == False:
       flag = True
    else:
        flag = False

def checkflag():
    if flag == True:
        flagvar.set("True")
        return True
    else:
        flagvar.set("False")
        return False
def addline():
    a = checkflag()
    if a:
        a = generatefunction()
        for x in range(1,len(a)):
            (x0,y0)=a[x]
            if x+1 < len(a):
                (x1,y1)=a[x+1]
            else:
                (x1,y1)=a[x]
                flipflag()
            drawline(x0,y0,x1,y1)
        recurse(x0,y0,x1,y1,50)


    else:
        return


flag = True

master = Tk()
master.title("Lines")
master.geometry("1280x720")

frame1 = Frame(master)
frame1.pack(expand = True, fill=BOTH)


button1 = Button(master, text = "start", command = addline)
button1.pack()
button1 = Button(master, text = "start/stop", command = flipflag)
button1.pack()


flagvar = StringVar()

label1 = Label(master, textvariable=flagvar)


flagvar.set("{}".format(checkflag()))

label1.pack()

canvas1 = Canvas(frame1, bg = 'white', width = 1230, height = 670)
#coordiantes go x0 y0 x1 y1

canvas1.pack(expand=True, fill=BOTH)



master.mainloop()

##
##for i in range(1,100):
##    list_a = []
##    for x in range(1, i):
##        if (i % x == 0):
##            # is divisible by x
##            list_a.append("{} and {}".format(x, i))
##        else:
##            continue
##    if len(list_a) == 1:
##        #check for numbers that were only divisible by one, these are prime
####        print ("{} are prime".format(list_a[0]))
##        print("{} is prime".format(i))
##    else:
####        print ("{} were not prime".format(list_a))
##        print("{} is not prime".format(i))
##
