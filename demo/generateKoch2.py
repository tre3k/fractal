#!/usr/bin/env python
# -*- coding: utf-8 -*-
#################################################################
#
# Koch Snowflake, Python, 2011-01-24
# 
# Artix, master@7masterov.ru, icq:53666599, skype:artixmaster
# 
# * Error in code? Nothing is perfect!
# * Free source for free Linux, use it for free!
# * Please, do not remove this comment!
#
#################################################################


from Tkinter import *
from math import *
import sys

SIZE_X=600
SIZE_Y=500
STEP=10
 
class Koch:
    
    def __init__(self,canvas):
        self.canvas = canvas

    def make(self,level):
        r = SIZE_Y/2-STEP
        x1 = SIZE_X/2
        y1 = SIZE_Y/2 - r*sin(pi/2)
        x2 = SIZE_X/2 + r*cos(pi/6)
        y2 = SIZE_Y/2 - r*sin(-pi/6)
        x3 = SIZE_X/2 - r*cos(-pi/6)
        y3 = SIZE_Y/2 - r*sin(-pi/6)
        self.segment(x1, y1, x2, y2, level)
        self.segment(x2, y2, x3, y3, level)
        self.segment(x3, y3, x1, y1, level)
        
    def segment(self,x1,y1,x2,y2,lev):
        if lev>0:
            angle = atan2(y2-y1, x2-x1);
            r = sqrt((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1));
 
            new_x1 = x1 + r * cos(angle) / 3;
            new_y1 = y1 + r * sin(angle) / 3;
            new_x2 = new_x1 + r * cos(angle - pi / 3) / 3;
            new_y2 = new_y1 + r * sin(angle - pi / 3) / 3;
            new_x3 = x1 + 2*r * cos(angle) / 3;
            new_y3 = y1 + 2*r * sin(angle) / 3;

            self.segment(x1, y1, new_x1, new_y1, lev-1);
            self.segment(new_x1, new_y1, new_x2, new_y2, lev-1);
            self.segment(new_x2, new_y2, new_x3, new_y3, lev-1);
            self.segment(new_x3, new_y3, x2, y2, lev-1);
        else:
            self.canvas.create_line(x1, y1, x2, y2, fill="green")
            


root = Tk()
root.title("Koch Snowflake")
c = Canvas(root,width=SIZE_X,height=SIZE_Y,bg="black")
c.pack()
s = Koch(c)
s.make(3)
 
root.mainloop()
