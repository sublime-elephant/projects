import random
from tkinter import *

def prime_check_plus_direction_change(n):
    prime_list = []
    for x in range(1,n):
        factor_list = []
        for i in range (1,x):
            if x % i == 0:
                factor_list.append(x)
        if len(factor_list) == 1:
            # print ("{} is prime".format(x))
            prime_list.append(x)

        # else:
            # print("{} not prime".format(x))

    return prime_list

def recursiveline(points, colour):
    line1 = canvas1.create_line(points[0], points[1], points[2], points[3], fill=colour, width=1)


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
        step = 1
        recurse_amount = 20000
        points = []
        points_mirror=[]
        next_points = [0,0]
        direction = 1
        prime_generate = prime_check_plus_direction_change(recurse_amount)
        print (prime_generate)
        #direction order 1,3,2,4
        next_points_mirror = [0,0]
        # points should have x0,y0,x1,y1
        for n in range(1,recurse_amount):
            if n == 1:
                points = [750, 480, 750+x, 480-y]
                points_mirror = [750, 480, 750-x, 480+y]
                recursiveline(points, "black")
                recursiveline(points_mirror, "black")
                next_points[0] = points[2]
                next_points[1] = points[3]
                next_points_mirror[0] = points_mirror[2]
                next_points_mirror[1] = points_mirror[3]
            else:
                if n in prime_generate:
                    print ("{} is prime, switching direction".format(n))
                    if direction == 1:
                        direction = 3
                    elif direction == 3:
                        direction = 2
                    elif direction == 2:
                        direction = 4
                    elif direction == 4:
                        direction = 1

                if direction == 1:
                    #R_UP
                    points = [next_points[0], next_points[1], int(next_points[0])+step, int(next_points[1])-step]
                if direction == 2:
                    #L_DOWN
                    points = [next_points[0], next_points[1], int(next_points[0])-step, int(next_points[1])+step]
                if direction == 3:
                    #L_UP
                    points = [next_points[0], next_points[1], int(next_points[0])-step, int(next_points[1])-step]
                if direction == 4:
                    #R_UP
                    points = [next_points[0], next_points[1], int(next_points[0])+step, int(next_points[1])+step]
                if direction == 1:
                    points_mirror = [next_points_mirror[0], next_points_mirror[1], int(next_points_mirror[0])-step, int(next_points_mirror[1])+step]
                if direction == 2:
                    points_mirror = [next_points_mirror[0], next_points_mirror[1], int(next_points_mirror[0])+step, int(next_points_mirror[1])-step]
                if direction == 3:
                    points_mirror = [next_points_mirror[0], next_points_mirror[1], int(next_points_mirror[0])+step, int(next_points_mirror[1])+step]
                if direction == 4:
                    points_mirror = [next_points_mirror[0], next_points_mirror[1], int(next_points_mirror[0])-step, int(next_points_mirror[1])-step]

                colour = random.randrange(1,4)
                if colour == 1:
                    recursiveline(points, "black")
                    recursiveline(points_mirror, "black")
                if colour == 2:
                    recursiveline(points, "red")
                    recursiveline(points_mirror, "black")
                if colour == 3:
                    recursiveline(points, "blue")
                    recursiveline(points_mirror, "black")
                next_points[0] = points[2]
                next_points[1] = points[3]
                next_points_mirror[0]=points_mirror[2]
                next_points_mirror[1]=points_mirror[3]
            # print (points, "these are the points")
            # print (next_points, "these are the next points")
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

canvas1 = Canvas(frame1, bg = 'white', width = 1280, height = 720)
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
