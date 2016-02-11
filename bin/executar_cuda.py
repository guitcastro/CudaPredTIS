#!/usr/bin/env python
# This Python file uses the following encoding: utf-8

import sys
import os
import fileHandle
import platform
import resultHandle

workingDir = sys.argv[4]

# for i in range (1,10):
#     commandLine = '(time ' + './cuda-means ' + \
#                    workingDir + 'cluster' + str(i) + \
#                    ' ' + sys.argv[2] + \
#                    ' ' + sys.argv[4] + 'cluster' + str(i) + \
#                    ') &> ' + sys.argv[4] +  'log' + str(i) + '.txt'
#     print commandLine
#     os.system(commandLine)
# 
# commandLine = '(time ' + './cuda-means ' + \
#     workingDir + 'cluster10' + \
#     ' ' + sys.argv[3] + \
#     ' ' + sys.argv[4] + 'cluster10' \
#     ') &> ' + sys.argv[4] +  'log10.txt'
# print commandLine
# os.system(commandLine)

resultHandle.evalResults(workingDir,10)

