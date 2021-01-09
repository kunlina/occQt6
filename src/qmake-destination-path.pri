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

platform_path = unknown-platform
compiler_path = unknown-compiler
processor_path = unknown-processor
build_path = unknown-build

PLATFORM_WIN {
    platform_path = windows
}
PLATFORM_OSX {
    platform_path = osx
}
PLATFORM_LINUX {
    platform_path = linux
}

COMPILER_GCC {
    compiler_path = gcc
}
COMPILER_MSVC2017 {
    compiler_path = msvc2017
}
COMPILER_CLANG {
    compiler_path = clang
}

PROCESSOR_x64 {
    processor_path = x64
}
PROCESSOR_x86 {
    processor_path = x86
}

BUILD_DEBUG {
    build_path = debug
} else {
    build_path = release
}

#DESTINATION_PATH = $$platform_path/$$compiler_path/$$processor_path/$$build_path
DESTINATION_PATH = $$platform_path/$$build_path
#message(Dest path: $${DESTINATION_PATH})
