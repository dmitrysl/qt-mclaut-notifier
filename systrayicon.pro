#-------------------------------------------------
#
# Project created by QtCreator 2015-03-29T10:01:32
#
#-------------------------------------------------

QT       += core gui network
CONFIG += static

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = systrayicon
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    helper.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    helper.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    res.qrc
