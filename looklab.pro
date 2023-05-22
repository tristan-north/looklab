HEADERS       = src/glwidget.h \
                src/mainwindow.h

SOURCES       = src/glwidget.cpp \
                src/main.cpp \
                src/mainwindow.cpp
                
INCLUDEPATH += src

QT           += widgets opengl

CONFIG += debug
TARGET = looklab
