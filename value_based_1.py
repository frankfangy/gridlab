# -*- coding: utf-8 -*- 
'''
这个来源于 udacity 的讲座 
验证基础的概念和算法

'''
from grid_world import *

import sys 
from PyQt5.QtCore import QPoint, QRect, QSize, Qt   
from PyQt5.QtGui import (QBrush,  QPainter, QColor, QPen )
from PyQt5.QtWidgets import (QApplication, QPushButton, QCheckBox, QGridLayout,QLabel, QWidget, QInputDialog)

import numpy as np 
from random import randint 
import time 


class value_based(grid_world):   
    '''
    用来实验各种算法，并图形展示，基于方块地图
    '''
    def __init__(self):  
        super(value_based, self).__init__()  
        


    def config_map(self):
        # 地图大小 , 初始map , 供子类再定义
        self.map_mask = '''
        1113
        1214
        1111
        ''' # map mask must be plus i8 (0 ~ 127 )


    def reset(self):  
        '''
        初始化地图，以及计算条件，可能会反复运算 
        '''
        # do nothing in base class 
        self.update()


    def run_proc(self):
        ''' 此处是一个示范代码，子类应沿袭相同结构 '''
        # setup flag
        self.running = True

        while self.running:
            # do your job
            pass 
        
        self.running = False  

        
    def draw_block(self, painter,x,y , block_map_value = None  ):
        painter.save()
        if block_map_value is None:
            block_map_value = self.map[x][y]
        
        bkcolor = {0:(55,55,55) , 1:(222,255,222) , 2 :(111,111,111) , 3:(111,255,111) , 4:(255,0,0) }.get( block_map_value , (0,0,0) )  
        self.draw_background( painter, x,y , bkcolor  )
        self.draw_text( painter , x,y ,  str(block_map_value) , (0,0,0) , 'd'  )
        self.draw_arrow(painter,x,y, 'urdl'[ randint(0,3) ] , (255,0,0) ,  'u'  )   
        painter.restore()




if __name__ == '__main__':  
    run_gui( value_based ) 
