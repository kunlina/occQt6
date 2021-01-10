##### MIT ####################################################################
# This file defines the compilation process for qmake                        #
#                                                                            #
# Copyright (c) 2021  Marius S. Schollmeier                                  #
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

include(src/qmake-target-platform.pri)
include(src/qmake-destination-path.pri)

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = occQt6
TEMPLATE = app

CASROOT = /usr/local/occt

# major, minor
VERSION = 1.0
DEFINES += VERSION_STRING=\\\"$${VERSION}\\\"

BINARYPATH = $$PWD/binaries/
DESTDIR = $$BINARYPATH/$$DESTINATION_PATH

BUILDPATH = $$PWD/build/BraggCalculator/
OBJECTS_DIR = $$BUILDPATH/$$DESTINATION_PATH/.obj
MOC_DIR = $$BUILDPATH/$$DESTINATION_PATH/.moc
RCC_DIR = $$BUILDPATH/$$DESTINATION_PATH/.qrc
UI_DIR = $$BUILDPATH/$$DESTINATION_PATH/.ui

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_DEBUG
    msvc{
        QMAKE_CFLAGS = /O2
        QMAKE_CXXFLAGS = /O2
        }
    else{
        QMAKE_CFLAGS -= -O2
        QMAKE_CFLAGS -= -O1
        QMAKE_CXXFLAGS -= -O2
        QMAKE_CXXFLAGS -= -O1
        QMAKE_CFLAGS = -m64 -O3
        QMAKE_LFLAGS = -m64 -O3
        QMAKE_CXXFLAGS = -m64 -O3
    }
}

SOURCES += \
    src/emptyspacerwidget.cpp \
    src/main.cpp \
    src/occtwidget.cpp \
    src/occview.cpp

HEADERS += \
    src/emptyspacerwidget.h \
    src/hirespixmap.h \
    src/occtwidget.h \
    src/occview.h


RESOURCES += \
    icons.qrc

macx {
    CASROOT = /usr/local/occt
    INCLUDEPATH += \
        $${CASROOT}/include/opencascade/

    LIBS += \
        -L$${CASROOT}/lib/

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
}

#LIBS +=         \
#    -lTKernel   \
#    -lTKMath    \
#    -lTKG3d     \
#    -lTKBRep    \
#    -lTKGeomBase\
#    -lTKGeomAlgo\
#    -lTKTopAlgo \
#    -lTKPrim    \
#    -lTKBO      \
#    -lTKBool    \
#    -lTKOffset  \
#    -lTKService \
#    -lTKV3d     \
#    -lTKOpenGl  \
#    -lTKFillet  \
#    -lTKXSDRAW

# occt foundation libs
LIBS += \
    -lTKernel   \
    -lTKMath    \

# occt visualization libs
LIBS += \
    -lTKOpenGl  \
    -lTKService \
    -lTKV3d \
    -lTKXSDRAW


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

