#-------------------------------------------------
#
# Project created by QtCreator 2017-11-21T17:46:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtFpgaSimulator
TEMPLATE = app

#INCLUDEPATH += `pkg-config --cflags jack`
LIBS += `pkg-config --libs jack` -lrt -lm -lpthread -lsndfile

SOURCES += main.cpp\
        window.cpp \
    jackclient.cpp \
    netclient.cpp \
    sender.cpp \
    soundfile.cpp \
    clibrary/byte-order.c

HEADERS  += window.h \
    clibrary/byte-order.h \
    clibrary/failure.h \
    clibrary/float.h \
    clibrary/int.h \
    clibrary/print.h \
    jackclient.h \
    netclient.h \
    sender.h \
    soundfile.h

FORMS    += window.ui
