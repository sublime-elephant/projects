import random
from tkinter import *


def recursiveline(points, colour):
    line1 = canvas1.create_line(points[0], points[1], points[2], points[3], fill=colour, width=1)
    master.update()


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
        #xys = [(y,((y))) for y in range(100)]
        x = 1
        y = 1
        step = 3
        points = []
        next_points = [0,0]
        # points should have x0,y0,x1,y1
        for n in range(1,10000):
            if n == 1:
                points = [615, 335, 615+x, 335-y]
                recursiveline(points, "black")
                next_points[0] = points[2]
                next_points[1] = points[3]
            else:
                #initialise which direction
                direction = random.randrange(1,5)
                if direction == 1:
                    points = [next_points[0], next_points[1], int(next_points[0])+step, int(next_points[1])-step]
                if direction == 2:
                    points = [next_points[0], next_points[1], int(next_points[0])-step, int(next_points[1])+step]
                if direction == 3:
                    points = [next_points[0], next_points[1], int(next_points[0])-step, int(next_points[1])-step]
                if direction == 4:
                    points = [next_points[0], next_points[1], int(next_points[0])+step, int(next_points[1])+step]

                colour = random.randrange(1,4)
                if colour == 1:
                    recursiveline(points, "red")
                if colour == 2:
                    recursiveline(points, "blue")
                if colour == 3:
                    recursiveline(points, "green")
                next_points[0] = points[2]
                next_points[1] = points[3]
            # print (points, "these are the points")
            # print (next_points, "these are the next points")
            x+=10
            y+=10
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

#
# for i in range(1,100):
#    list_a = []
#    for x in range(1, i):
#        if (i % x == 0):
#            # is divisible by x
#            list_a.append("{} and {}".format(x, i))
#        else:
#            continue
#    if len(list_a) == 1:
#        #check for numbers that were only divisible by one, these are prime
# ##        print ("{} are prime".format(list_a[0]))
#        print("{} is prime".format(i))
#    else:
# ##        print ("{} were not prime".format(list_a))
#        print("{} is not prime".format(i))
