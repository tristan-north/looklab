HEADERS       = src/glwidget.h \
                src/mainwindow.h \
                src/logo.h
SOURCES       = src/glwidget.cpp \
                src/main.cpp \
                src/mainwindow.cpp \
                src/logo.cpp
                
INCLUDEPATH += src

QT           += widgets

CONFIG += debug
TARGET = looklab
