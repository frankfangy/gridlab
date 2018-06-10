# -*- coding: utf-8 -*- 
'''
这个来源于 udacity 的讲座 
验证基础的概念和算法

'''
from grid_world import *

import sys 
#from PyQt5.QtCore import QPoint, QRect, QSize, Qt   
#from PyQt5.QtGui import (QBrush,  QPainter, QColor, QPen )
#from PyQt5.QtWidgets import (QApplication, QPushButton, QCheckBox, QGridLayout,QLabel, QWidget, QInputDialog)

from PyQt5.QtWidgets import ( QLabel , QTextEdit , QPlainTextEdit , QLineEdit  )

import numpy as np 
from random import randint 
import time 
from math import fabs


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
 

    def config_gui(self):
        super(value_based, self).config_gui()
 
        self.add_ctrl( QLabel , '移动奖励' )
        self.edit_moving_reward = self.add_ctrl( QLineEdit , '-0.04' , 15 )

        self.add_ctrl( QLabel , '绿位奖励' )
        self.edit_green_reward = self.add_ctrl( QLineEdit , '+1' , 15 )

        self.add_ctrl( QLabel , '红位奖励' )
        self.edit_red_reward = self.add_ctrl( QLineEdit , '-1' , 15 )

        self.add_ctrl( QLabel , '运动稳定度' )
        self.edit_moving_stability = self.add_ctrl( QLineEdit , '0.8' , 15 )

        self.block_size = 60 # using a bigger block

    def reset(self):  
        '''
        初始化地图，以及计算条件，可能会反复运算 
        '''
        self.moving_reward = float( self.edit_moving_reward.text() )
        self.green_reward = float( self.edit_green_reward.text() )
        self.red_reward = float( self.edit_red_reward.text() )
        self.moving_stability = float( self.edit_moving_stability.text() )
        print('get config :',self.moving_reward , self.green_reward , self.red_reward , self.moving_stability )

        # init value
        self.value_init = -100
        self.value = np.zeros( ( self.map_width , self.map_height ) , float )
        for x in range(self.map_width):
            for y in range(self.map_height):
                mpv = self.map[x][y]
                self.value[x][y] = {1:self.value_init , 2:self.value_init , 3: self.green_reward , 4:self.red_reward }.get(mpv,0)

        self.show_direction = False 
        self.update()

    def avaliable_for_value(self,x,y):
        if x < 0 or x >= self.map_width or y < 0 or y >= self.map_height:
            return False 
        return self.map[x][y] != 2

    def calc_direction(self):
        self.direction = np.ndarray( ( self.map_width , self.map_height ) , dtype=np.int8 )
        for x in range(self.map_width):
            for y in range(self.map_height):                
                maxv = self.value_init
                ii = 4 # stay there
                if self.avaliable_for_value(x,y):
                    v0 = self.value[x][y]
                    for i in range(5):
                        d = ((0,-1),(1,0),(0,1),(-1,0),(0,0))[i]
                        nx = x + d[0]
                        ny = y + d[1]
                        if self.avaliable_for_value(nx,ny):
                            if maxv < self.value[nx][ny]  :
                                ii = i 
                                maxv = self.value[nx][ny] 
                    self.direction[x][y] = ii 
        self.show_direction = True 
     
    def run_proc(self):
        ''' 此处是一个示范代码，子类应沿袭相同结构 '''
        # setup flag
        self.running = True
        self.set_log('运算中')

        while self.running:
            updated = 0
            self.value_old = self.value.copy()
            for x in range(self.map_width):
                for y in range(self.map_height):
                    if  self.map[x][y] == 1:
                        v0 = self.value_old[x][y]
                        maxv = self.value_init 
                        for i in range(4):
                            d  =((0,-1),(1,0),(0,1),(-1,0))[i]
                            d1 =((-1,0),(0,-1),(1,0),(0,1))[i]
                            d2 =((1,0),(0,1),(-1,0),(0,-1))[i]
                            nx = x + d[0]
                            ny = y + d[1]           
                            nx1 = x + d1[0]
                            ny1 = y + d1[1]           
                            nx2 = x + d2[0]
                            ny2 = y + d2[1]                            
                            if self.avaliable_for_value(nx,ny): # this nx,ny is avaliable for value
                                # 计算，如果向 nx ny 点移动， 本地的value值可能是多少 
                                v = self.value_old[nx][ny] * self.moving_stability 
                                if self.avaliable_for_value(nx1,ny1):
                                    v += (1.0-self.moving_stability )*0.5 * self.value_old[nx1][ny1]
                                else:  # 留在原地
                                    v += (1-self.moving_stability )*0.5 * v0

                                if self.avaliable_for_value(nx2,ny2):
                                    v += (1-self.moving_stability )*0.5 * self.value_old[nx2][ny2]
                                else:  # 留在原地
                                    v += (1-self.moving_stability )*0.5 * v0
                                if v > maxv:
                                    maxv = v 
                        # here we got the maxv 
                        if   v0 - self.moving_reward < maxv: 
                            updated += fabs( v0 - maxv - self.moving_reward )
                            self.value[x][y] = maxv + self.moving_reward
            if updated < 1e-5 :
                break 
            self.update()
            time.sleep(0.3)

        self.set_log('价值网络计算完成')   
        self.calc_direction()     
        self.running = False  
        self.update()

        
    def draw_block(self, painter,x,y , block_map_value = None  ):
        painter.save()
        if block_map_value is None:
            block_map_value = self.map[x][y]
        block_value = self.value[x][y]
        
        bkcolor = {0:(55,55,55) , 1:(222,255,222) , 2 :(111,111,111) , 3:(111,255,111) , 4:(255,0,0) }.get( block_map_value , (0,0,0) )  
        self.draw_background( painter, x,y , bkcolor  )
        if block_map_value == 1: # path block
            self.draw_text( painter , x,y ,  '%g'%(block_value) , (0,0,0) , 'd'  )
            if self.show_direction:
                self.draw_arrow(painter,x,y, 'urdlo'[ self.direction[x][y] ] , (255,0,0) ,  'u'  )  
        elif block_map_value != 2: # other
            self.draw_text( painter , x,y ,  str(block_value) , (0,0,0) , 'd'  )

        painter.restore()




if __name__ == '__main__':  
    run_gui( value_based ) 
