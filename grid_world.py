# -*- coding: utf-8 -*- 
'''
to run this , sudo apt-get install python3-pyqt5
'''
import sys 
from PyQt5.QtCore import QPoint, QRect, QSize, Qt   
from PyQt5.QtGui import (QBrush,  QPainter, QColor, QPen )
from PyQt5.QtWidgets import (QApplication, QPushButton, QCheckBox, QGridLayout,QLabel, QWidget, QInputDialog)

import numpy as np 
from random import randint 
import time 
import threading 

###############################################

class grid_world(QWidget):   
    '''
    用来实验各种算法，并图形展示，基于方块地图
    '''
    def __init__(self):  
        super(grid_world, self).__init__()  
        self.initUI()  


    # ----------------------------- 子类继承，一般也需要调用基类函数的部分 ------------------------------------ 
    # ----- super( cls_name , self).config_gui() ...

    def config_gui(self):
        # 界面组件 init & run 
        font = self.font()
        font.setPointSize(8)
        self.setFont(font)      
        self.pen = QPen()
        self.brush = QBrush()
        self.btn = QPushButton('init', self)  
        self.btn.move(10, 10)  
        self.btnrun = QPushButton('run', self)  
        self.btnrun.move(10, 40)  
        self.btnstop = QPushButton('stop', self)  
        self.btnstop.move(10, 70)  
        self.bkcolor = QColor(22,111,22)
        self.log_text = '欢迎来到网格世界'
        
        self.btn.clicked.connect(self.reset) 
        self.btnrun.clicked.connect(self.run)
        self.btnstop.clicked.connect(self.stop)

        # 方块大小 , 基础的绘制布局配置
        self.block_size = 40
        self.map_topleft = (100,10)
        
    def calc_map(self):        
        ''' 根据之前的设置，计算整体大小 , 初始化 map '''
        mask = self.map_mask.strip()
        mask = mask.replace('\t','')
        mask = mask.replace(' ','')
        mask = mask.split('\n')

        self.map_height = len(mask)
        self.map_width = max([ len(x) for x in mask ]) 

        self.map = np.ndarray( ( self.map_width , self.map_height ) , dtype=np.int8 )
        for y in range(self.map_height):
            if len(mask[y]) == self.map_width:
                for x in range(self.map_width):
                    self.map[x][y] = int( mask[y][x] )
            else:
                for x in range(len(mask[y])):
                    self.map[x][y] = int( mask[y][x] )
                x = len(mask[y])
                while x < self.map_width:
                    self.map[x][y] = 0 # 0 as default

    # -------------------------- 子类一般重写的部分 , 也不调用基类部分 --------------------
    def config_map(self):
        # 地图大小 , 初始map , 供子类再定义
        self.map_mask = '''
        1111 1111 1111 1111
        1122 1212 1331 2212
        1122 1212 1331 2212
        1122 1212 1331 2212
        1122 1212 1331 2212
        1122 1212 1331 2212
        2111 1111 1222 2112
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
        
        bkcolor = {0:(55,55,55) , 1:(222,255,222) , 2 :(111,111,111) , 3:(111,255,111) }.get( block_map_value , (0,0,0) )  
        self.draw_background( painter, x,y , bkcolor  )
        self.draw_text( painter , x,y ,  str(block_map_value) , (0,0,0) , 'd'  )
        self.draw_arrow(painter,x,y, 'urdl'[ block_map_value ] , (255,0,0) ,  'u'  )   
        painter.restore()

    
    # --------------------------------- 基本不碰的部分 -----------------------------
    def initUI(self):  
        # 界面组件 init & run 
        self.config_gui()

        # 地图大小 , 初始map , 供子类再定义
        self.config_map()
       

        self.calc_map() # 解析 map_mask

        self.window_width = self.map_topleft[0] + self.block_size * self.map_width + 10
        self.window_height = self.map_topleft[1] + self.block_size * self.map_height + 10 + 25
        if self.window_width < 300:
            self.window_width = 300 
        if self.window_height < 200:
            self.window_height = 200 

        
        self.setGeometry(300, 300, self.window_width , self.window_height )  
        self.setWindowTitle('grid_world')  
        self.show()  

        self.running_thread = None 
        self.running = False 
        self.reset()

        
    def run(self):
        ''' 运行算法 '''
        self.running_thread = threading.Thread( target=self.run_proc )
        self.running_thread.start() 
        
    def stop(self):
        if self.running_thread is not None:
            self.running = False 
            self.running_thread.join()
            self.running_thread = None 
            
    def set_log(self,text):        
        self.log_text = text 
        self.update()

    def get_grid_range(self,x,y):
        ''' 
        输入，逻辑网格的坐标 x,y , 
        返回 网格区域的 范围 top_left_x , top_left_y , width , height 
        '''
        return  self.map_topleft[0] + self.block_size * x,self.map_topleft[1] + self.block_size * y , self.block_size ,self.block_size

    def draw_background(self,painter, x,y, color ):
        ''' 
        绘制特定颜色的 格子的背景 
        输入：
        x,y 逻辑网格的坐标
        color : QColor 或者 (r,g,b)
        '''
        painter.setPen(self.bkcolor)
        if isinstance( color , QColor ):
            painter.setBrush( color )
        else:
            painter.setBrush(QColor( color[0] , color[1] , color[2] ))
        x,y,cx,cy = self.get_grid_range( x,y )
        painter.drawRect(x, y, cx-1, cy-1)  

    def draw_text(self, painter , x, y ,  text , color = None,  pos_flag ='c'  ):
        '''
        pos_flag :  'c' : 整个格子正中央
                    'l' :  格子 左侧
                    'r' , 'u' , 'd' ... 
        x,y : 网格地图逻辑坐标
        color : None 不改变当前颜色 ， QColor 或 (r,g,b) 则以特定颜色绘制
        '''        
        x,y,cx,cy = self.get_grid_range( x,y )
        rg = QRect( x,y,cx-1,cy-1)
        if pos_flag == 'c': 
            pass 
        elif pos_flag == 'l':            
            rg = QRect( x,y,cx/2,cy-1)  
        elif pos_flag == 'r':            
            rg =  QRect( x+cx/2,y,cx/2,cy-1) 
        elif pos_flag == 'u':            
            rg =  QRect( x,y,cx-1,cy/2)                                  
        elif pos_flag == 'd':            
            rg =  QRect( x,y+cy/2,cx-1,cy/2)  
        
        if color is not None:
            if isinstance( color , QColor ):                
                painter.setPen( color )
            else:
                painter.setPen(QColor( color[0] , color[1] , color[2] ))
        painter.drawText( rg  ,   Qt.AlignCenter, text )

    
    def draw_arrow(self, painter , x, y ,  direction , color = None,  pos_flag ='c'  ):   
        '''
        direction
        pos_flag :  'c' : 整个格子正中央
                    'l' :  格子 左侧
                    'r' , 'u' , 'd' ... 
        x,y : 网格地图逻辑坐标
        color : None 不改变当前颜色 ， QColor 或 (r,g,b) 则以特定颜色绘制
        '''             
        x,y,cx,cy = self.get_grid_range( x,y )
        centerx = x + cx//2 
        centery = y + cy//2
        maxsz = min((cx,cy))

        if pos_flag == 'c': 
            pass 
        elif pos_flag == 'l':            
            centerx = x + cx//4
            maxsz = min((cx//2,cy))
        elif pos_flag == 'r':            
            centerx = x + cx//2 + cx//4
            maxsz = min((cx//2,cy))
        elif pos_flag == 'u':                    
            centery = y + cy//4
            maxsz = min((cx,cy//2))                              
        elif pos_flag == 'd':                     
            centery = y + cy//2 + cy//4
            maxsz = min(cx,cy//2)

        maxsz = maxsz * 0.7
        arsz = maxsz//4
        if arsz < 2:
            arsz = 2 
            
        if color is not None:
            if isinstance( color , QColor ):                
                painter.setPen( color )
            else:
                painter.setPen(QColor( color[0] , color[1] , color[2] ))

        if direction == 'l':
            pt1 = (  centerx + maxsz//2 , centery )
            pt2 = (  centerx - maxsz//2 , centery )
            painter.drawLine( pt1[0] , pt1[1] , pt2[0] , pt2[1]  )
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] + arsz , pt2[1] - arsz)
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] + arsz , pt2[1] + arsz)
        elif direction == 'r':
            pt2 = (  centerx + maxsz//2 , centery )
            pt1 = (  centerx - maxsz//2 , centery )
            painter.drawLine( pt1[0] , pt1[1] , pt2[0] , pt2[1]  )
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] - arsz , pt2[1] - arsz)
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] - arsz , pt2[1] + arsz)
        elif direction == 'u':
            pt2 = (  centerx  , centery- maxsz//2 )
            pt1 = (  centerx  , centery+ maxsz//2  )
            painter.drawLine( pt1[0] , pt1[1] , pt2[0] , pt2[1]  )
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] + arsz , pt2[1] + arsz)
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] - arsz , pt2[1] + arsz)
        else:
            pt1 = (  centerx  , centery- maxsz//2 )
            pt2 = (  centerx  , centery+ maxsz//2  )
            painter.drawLine( pt1[0] , pt1[1] , pt2[0] , pt2[1]  )
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] + arsz , pt2[1] - arsz)
            painter.drawLine( pt2[0] , pt2[1] , pt2[0] - arsz , pt2[1] - arsz)


                
    def drawMap(self, painter):
        ''' draw the map based on self.map '''
        painter.save()

        # draw back ground        
        painter.setBrush(self.bkcolor)
        painter.drawRect( self.map_topleft[0] - 2 , self.map_topleft[1] - 2,
                          self.block_size * self.map_width + 4 ,
                          self.block_size * self.map_height + 4 )  

        for x in range(self.map_width):
            for y in range(self.map_height):
                self.draw_block(painter,x,y, self.map[x][y] )
        painter.restore()
        
    def drawLog(self, painter):
        painter.save()
    
        painter.setBrush(QColor( 255,255,255 ))
        painter.drawRect( 10 , self.window_height - 22 , self.window_width - 20 , 15  )
        
        logrg2  = QRect( 12 , self.window_height - 22 + 3 , self.window_width - 20 -4 , 12 )
        painter.drawText( logrg2, Qt.AlignLeft , self.log_text )
        painter.restore()

    def paintEvent(self, event):
        '绘制界面'
        self.cx = self.width()
        self.cy = self.height() 
        painter = QPainter(self)
        painter.save()
        
        self.drawMap(painter)
        try:
            self.drawMap(painter)
            self.drawLog(painter)
        except Exception as e:
            painter.setPen(QColor( 255,0,0 ))
            painter.drawText( QRect( self.map_topleft[0] , self.map_topleft[1] , 400,300  ) ,  Qt.AlignCenter,
                                      "paintEvent met exception:\n" + repr(e) + str(e)  )
     

        painter.restore()
             

def run_gui( gui_cls ):
    app = QApplication(sys.argv)  
    ex = gui_cls()  
    sys.exit(app.exec_())  

 

if __name__ == '__main__':  
    run_gui( grid_world ) 
