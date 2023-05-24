HEADERS       = src/glwidget.h \
                src/geo.h \
                src/mainwindow.h

SOURCES       = src/glwidget.cpp \
                src/geo.cpp \
                src/main.cpp \
                src/mainwindow.cpp
                
INCLUDEPATH += src /usr/local/include/Imath
LIBS += -L/usr/local/lib -lAlembic

QT           += widgets opengl

CONFIG += debug
TARGET = looklab
