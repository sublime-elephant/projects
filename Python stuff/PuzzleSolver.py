board = [
    [],[],[],
    [],[],[],
    [],[],[]
    ]

originaltilelist=[
    [1,0,[1,0],[4,0],[3,1],[2,1]], #tile 1
    [2,0,[1,0],[4,1],[2,0],[2,0]], #tile 2
    [3,0,[2,0],[4,0],[1,0],[3,0]], #tile 3
    [4,0,[2,1],[3,1],[4,0],[1,0]], #...
    [5,0,[1,1],[3,1],[4,1],[4,0]],
    [6,0,[3,0],[2,1],[3,1],[1,1]],
    [7,0,[4,1],[1,0],[2,1],[3,1]],
    [8,0,[3,0],[2,0],[1,1],[4,1]],
    [9,0,[3,0],[4,1],[1,1],[2,1]]
    ]

def rotate_right(tile):
        tile.insert(2, tile.pop(5))
        tile[1]+=1
        return tile

def rotate_left(tile):
    tile.insert(5,tile.pop(2))
    tile[1]-=1
    return tile

def unspin_tiles(tilelist):
     for x in range(0,len(tilelist)):
            a = tilelist[x][1]
            b=0
            while (b < a):
                 rotate_left(tilelist[x])
                 b+=1
#connlistx=list(tilenumber(rotation(up(right(down(left))))))
connectionlist=[[[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                [[[],[],[],[]],[[],[],[],[]],[[],[],[],[]],[[],[],[],[]]],
                ] 
tilelist=originaltilelist
# [0][1][2]       2
# [3][4][5]     5   3
# [6][7][8]       4
def go(tileno):
    h=0
    corrects=0
    for q in range(0,len(tilelist)):
        tile_selected = tilelist.pop(tileno)
        for p in range(0,4):
            for y in range(0,4):
                for x in range(0, len(tilelist)): #up
                    h+=1
                    if (tile_selected[2][0] == tilelist[x][4][0]) and (tile_selected[2][1]^tilelist[x][4][1]):
                        corrects+=1
                        connectionlist[tile_selected[0]-1][tile_selected[1]][0].append("{}+{}".format(tilelist[x][0],tilelist[x][1]))
                for x in range(0, len(tilelist)): #right
                    h+=1
                    if (tile_selected[3][0] == tilelist[x][5][0]) and (tile_selected[3][1]^tilelist[x][5][1]):
                        corrects+=1
                        connectionlist[tile_selected[0]-1][tile_selected[1]][1].append("{}+{}".format(tilelist[x][0],tilelist[x][1]))
                for x in range(0, len(tilelist)): #down
                    h+=1
                    if (tile_selected[4][0] == tilelist[x][2][0]) and (tile_selected[4][1]^tilelist[x][2][1]):
                        corrects+=1
                        connectionlist[tile_selected[0]-1][tile_selected[1]][2].append("{}+{}".format(tilelist[x][0],tilelist[x][1]))
                
                for x in range(0, len(tilelist)): #left
                    h+=1
                    if (tile_selected[5][0] == tilelist[x][3][0]) and (tile_selected[5][1]^tilelist[x][3][1]):
                        corrects+=1
                        connectionlist[tile_selected[0]-1][tile_selected[1]][3].append("{}+{}".format(tilelist[x][0],tilelist[x][1]))
                    rotate_right(tilelist[x]) 
            unspin_tiles(tilelist)
            rotate_right(tile_selected)
        tilelist.insert(tileno,tile_selected)
        tileno+=1
        unspin_tiles(tilelist)
    return(connectionlist)

def makesquare(connectionlist):
    squaresc=0
    squares=[]
    for x in range(0, len(connectionlist)):
        for y in range(0, len(connectionlist[x])):
                firstTilePossibilitiesRight=[]
                firstTilePossibilitiesDown=[]
                for p in range(0,len(connectionlist[x][y][1])):
                    firstTilePossibilitiesRight.append("%d+%d"%(int(connectionlist[x][y][1][p][0]),int(connectionlist[x][y][1][p][2])))
                    firstTilePossibilitiesRight.append(connectionlist[int(connectionlist[x][y][1][p][0])-1][int(connectionlist[x][y][1][p][2])][2])
                    


                for k in range(0,len(connectionlist[x][y][2])):
                    firstTilePossibilitiesDown.append("%d+%d"%(int(connectionlist[x][y][2][k][0]),int(connectionlist[x][y][2][k][2])))
                    firstTilePossibilitiesDown.append(connectionlist[int(connectionlist[x][y][2][k][0])-1][int(connectionlist[x][y][2][k][2])][1])

                for j in range(0,len(firstTilePossibilitiesRight)):
                    if type(firstTilePossibilitiesRight[j]) == list:
                        for h in range(0,len(firstTilePossibilitiesRight[j])):
                            for t in range(0,len(firstTilePossibilitiesDown)):
                                if type(firstTilePossibilitiesDown[t]) == list:
                                    for q in range(0,len(firstTilePossibilitiesDown[t])):
                                            if firstTilePossibilitiesRight[j][h] == firstTilePossibilitiesDown[t][q]:
                                                g=set()
                                                g.add(int(x+1))
                                                g.add(int(firstTilePossibilitiesRight[j-1][0]))
                                                g.add(int(firstTilePossibilitiesDown[t-1][0]))
                                                g.add(int(firstTilePossibilitiesRight[j][h][0]))
                                                print(g)
                                                if len(g) == 4:
                                                    squares.append("{}+{},{},{},{}".format(x+1,y,firstTilePossibilitiesRight[j-1], firstTilePossibilitiesDown[t-1], firstTilePossibilitiesRight[j][h]))
                                                    squaresc+=1
                                                del(g)
    for x in range(0,len(squares)):
        squares[x]=squares[x].split(",")
    print(squares)
    print(squaresc)
    for e in range(0,len(squares)):
        for d in range(0,len(squares)):
            for n in range(0,len(squares)):
                for r in range(0,len(squares)):
                    if (squares[e][1] == squares[d][0]) and (squares[e][2] == squares[n][0]) and (squares[e][3]==squares[r][0]) and (squares[d][2] == squares[r][0]) and (squares[n][3] == squares[r][2]) and (squares[d][3]==squares[r][1]):
                        v = set()
                        for y in range(0,4):
                            v.add(squares[e][y][0])
                            v.add(squares[d][y][0])
                            v.add(squares[n][y][0])
                            v.add(squares[r][y][0])
                        if len(v) == 9:
                            print("{},{},{},{}".format(squares[e],squares[d],squares[n],squares[r]))
                            input()
                          
conn = go(0)
makesquare(conn)