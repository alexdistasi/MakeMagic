#-------------------------------------------------
#
# Project created by QtCreator 2016-05-02T06:59:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MakeMagic
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    image.cpp

HEADERS  += mainwindow.h \
    image.h

FORMS    += mainwindow.ui

LIBS += `pkg-config opencv --libs`
