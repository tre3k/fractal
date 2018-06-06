#!/usr/bin/python3

X=2048
Y=2048
N = 3

data = [0]*X
for i in range(X):
    data[i] = [0]*Y

def printData():
    fw = open("fractal_serpinsky"+str(X)+"x"+str(Y)+"_"+str(N)+".txt",'w')
    print(X,"x",Y)
    print("N =",N)
    print("to file:","fractal_serpinsky"+str(X)+"x"+str(Y)+"_"+str(N)+".txt")
    fw.write(str(X)+'\n'+str(Y)+'\n')
    for i in range(0,X):
        for j in range(0,Y):
            fw.write(str(data[i][j])+'\n')
    return 

def paintRect(x1,y1,x2,y2):
    for i in range(int(x1),int(x2)):
        for j in range(int(y1),int(y2)):
            data[i][j] = 1
    return

def serp(x1,y1,x2,y2,n):
    if n > 0:
        x1n = 2*x1/3 + x2/3;
        x2n = x1/3 + 2*x2/3;
        y1n = 2*y1/3 + y2/3;
        y2n = y1/3 + 2*y2/3;
        paintRect(x1n,y1n,x2n,y2n)
        serp(x1,y1,x1n,y1n,n-1)
        serp(x1n,y1,x2n,y1n,n-1)
        serp(x2n,y1,x2,y1n,n-1)
        serp(x1,y1n,x1n,y2n,n-1)
        serp(x2n,y1n,x2,y2n,n-1)
        serp(x1,y2n,x1n,y2,n-1)
        serp(x1n,y2n,x2n,y2,n-1)
        serp(x2n,y2n,x2,y2,n-1)
    return

def main():
    serp(0,0,X,Y,N)
    printData();
    return

main()
