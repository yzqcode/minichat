#-------------------------------------------------
#
# Project created by QtCreator 2016-06-11T16:05:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = minichat
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp \
    Cuser.cpp \
    CepollServer.cpp

HEADERS += \
    stdafx.h \
    Cuser.h \
    CepollServer.h
QMAKE_CXXFLAGS += -std=c++11
