#-------------------------------------------------
#
# Project created by QtCreator 2015-03-29T10:01:32
#
#-------------------------------------------------

QT       += core gui network
CONFIG += static c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = McLautAccountInfo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    helper.cpp \
    utils.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    helper.h \
    utils.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    res.qrc
