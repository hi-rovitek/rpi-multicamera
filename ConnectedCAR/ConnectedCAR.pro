#-------------------------------------------------
#
# Project created by QtCreator 2017-11-16T15:24:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConnectedCAR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

INCLUDEPATH += /usr/local/opencvARM/include/

LIBS +=  -L/usr/local/opencvARM/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lwiringPi -lpthread


HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
