#!/usr/bin/python3

import os
from math import *

X=1024
Y=1024
N = 4

X0 = (X-1)/2
Y0 = (Y-1)/2

A = X

data = [0]*X
for i in range(X):
    data[i] = [0]*Y

def printData():
    fw = open("fractal_koch"+str(X)+"x"+str(Y)+"_"+str(N)+".txt",'w')
    print(X,"x",Y)
    print("N =",N)
    print("to file:","fractal_koch"+str(X)+"x"+str(Y)+"_"+str(N)+".txt")
    fw.write(str(X)+'\n'+str(Y)+'\n')
    for i in range(0,X):
        for j in range(0,Y):
            fw.write(str(data[i][j])+'\n')
    return 

def paintTrig(a,x0,y0,ang):
    k = tan(pi/3)
    
    x0 = X0+x0
    #y0 = Y0+y0
    #y0 = y0+0.866*a/2+Y0
    if ang==0:
        y0 = y0+0.866*a/2+Y0
        for i in range(int(y0-0.866*a),int(y0+0.866*a)):
            for j in range(int(x0-((y0-i)/k)),int(x0+((y0-i)/k))):
                data[i][j] = 1
    if ang==1:
        y0 = -y0-0.866*a/2+Y0
        for i in range(int(y0-0.866*a),int(y0+0.866*a)):
            for j in range(int(x0+((y0-i)/k)),int(x0-((y0-i)/k))):
                data[i][j] = 1
    
    return

def Koch(A,n,x0,y0):
    if n==0:
        return
    A = A/3
    a = A
    ang = n%2
    paintTrig(a,x0,y0,ang)
    paintTrig(a,x0,(y0+0.866*a-0.866*a/3),ang)
    paintTrig(a,x0,-(y0+0.866*a-0.866*a/3),ang)
    Koch(a,n-1,x0+a/3,y0)
    Koch(a,n-1,x0-a/3,y0)
    Koch(a,n-1,x0,(y0+0.866*a-0.866*a/3))  

    return


def main():
    Koch(A,N,0,0)
    #paintTrig(A/3,0,300,0)
    printData()
    return

main()
