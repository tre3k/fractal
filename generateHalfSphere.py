#!/usr/bin/python3

import math as m

X = 2048
Y = 2048

radius = 64

data = [0] * X
for i in range(X):
    data[i] = [0] * Y

def printData():
    fw = open("half-sphere-"+str(X)+"x"+str(Y)+"_r"+str(radius)+".txt",'w')
    print(X,"x",Y)
    print("to file:","half-sphere-"+str(X)+"x"+str(Y)+".txt")
    fw.write(str(X)+'\n'+str(Y)+'\n')
    for i in range(0,X):
        for j in range(0,Y):
            fw.write(str(data[i][j])+'\n')
    return


for i in range(X):
    for j in range(Y):
        x = i - X/2
        y = j - Y/2
        core =  radius**2 - x**2 - y**2
        if core >= 0:
            data[i][j] = m.sqrt(core)

printData()
