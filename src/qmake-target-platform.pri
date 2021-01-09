##############################################################################
# Preprocessor definitions for qmake. Adapted from the book "End to End GUI  #
# Development with Qt5" by Nicholas Sherriff, Guillaume Lazar, Robin Penea,  #
# and Marco Piccolino.                                                       #
# Copyright (c) 2018 Packt Publishing.                                       #
#                                                                            #
# Permission is hereby granted, free of charge, to any person obtaining a    #
# copy of this software and associated documentation files (the "Software"), #
# to deal in the Software without restriction, including without limitation  #
# the rights to use, copy, modify, merge, publish, distribute, sublicense,   #
# and/or sell copies of the Software, and to permit persons to whom the      #
# Software is furnished to do so, subject to the following conditions:       #
#                                                                            #
# The above copyright notice and this permission notice shall be included    #
# in all copies or substantial portions of the Software.                     #
#                                                                            #
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS    #
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                 #
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     #
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY       #
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,       #
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE          #
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                     #
#                                                                            #
##############################################################################
# Author:  Marius Schollmeier                                                #
# Contact: mschollmeier01@gmail.com                                          #
# Date:    Jan. 01, 2021                                                     #
# Version: 1.0                                                               #
##############################################################################


win32 {
       CONFIG += PLATFORM_WIN
       #message(PLATFORM_WIN)
       win32-g++ {
           CONFIG += COMPILER_GCC
           win32-g++:QMAKE_TARGET.arch = x86_64
           #message(COMPILER_GCC)
       }
       win32-msvc2017 {
           CONFIG += COMPILER_MSVC2017
           #message(COMPILER_MSVC2017)
           win32-msvc2017:QMAKE_TARGET.arch = x86_64
        }
}

linux {
       CONFIG += PLATFORM_LINUX
       #message(PLATFORM_LINUX)
       # Make QMAKE_TARGET arch available for Linux
       !contains(QT_ARCH, x86_64){
           QMAKE_TARGET.arch = x86
       }
       else {
           QMAKE_TARGET.arch = x86_64
       }
       linux-g++{
           CONFIG += COMPILER_GCC
           #message(COMPILER_GCC)
       }
}

macx {
       CONFIG += PLATFORM_OSX
       QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
       #message(PLATFORM_OSX)
       macx-clang {
           CONFIG += COMPILER_CLANG
           #message(COMPILER_CLANG)
           QMAKE_TARGET.arch = x86_64
       }
       macx-clang-32{
           CONFIG += COMPILER_CLANG
           #message(COMPILER_CLANG)
           QMAKE_TARGET.arch = x86
       }
}

contains(QMAKE_TARGET.arch, x86_64) {
    CONFIG += PROCESSOR_x64
    #message(PROCESSOR_x64)
}
else {
    CONFIG += PROCESSOR_x86
    message("WARNING! 32-bit build! PROCESSOR_x86")
}

CONFIG(debug, release|debug) {
    CONFIG += BUILD_DEBUG
#    message(BUILD_DEBUG)
}
else {
    CONFIG += BUILD_RELEASE
#    message(BUILD_RELEASE)
}

