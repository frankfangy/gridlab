# -*- coding: utf-8 -*- 
'''
这个来源于 udacity 的讲座 
验证基础的概念和算法

'''
from value_based_1 import *

import sys 
#from PyQt5.QtCore import QPoint, QRect, QSize, Qt   
#from PyQt5.QtGui import (QBrush,  QPainter, QColor, QPen )
#from PyQt5.QtWidgets import (QApplication, QPushButton, QCheckBox, QGridLayout,QLabel, QWidget, QInputDialog)

from PyQt5.QtWidgets import ( QLabel , QTextEdit , QPlainTextEdit , QLineEdit  )

import numpy as np 
from random import randint 
import time 
from math import fabs


class value_based_2(value_based):   
    '''
    用来实验各种算法，并图形展示，基于方块地图
    '''
    def __init__(self):  
        super(value_based_2, self).__init__()  
        
    def config_map(self):
        # 地图大小 , 初始map , 供子类再定义
        self.map_mask = '''
        1113111211
        1214141311
        1111111114
        1114111111
        ''' # map mask must be plus i8 (0 ~ 127 )
 

if __name__ == '__main__':  
    run_gui( value_based_2 ) 
