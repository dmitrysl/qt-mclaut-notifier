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
    utils.cpp \
    appsettingsstorage.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    helper.h \
    utils.h \
    program_details.h \
    appsettingsstorage.h

FORMS    += mainwindow.ui

DISTFILES +=

RESOURCES += \
    res.qrc
	
win32:VERSION = 1.2.3.4 # major.minor.patch.build
else:VERSION = 1.2.3    # major.minor.patch

win32:RC_FILE=app.rc
