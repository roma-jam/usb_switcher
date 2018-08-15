#-------------------------------------------------
#
# Project created by QtCreator 2018-08-15T11:55:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = USB_Relay
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    hid.cpp \
    hid_tree.cpp

HEADERS  += mainwindow.h \
    config.h \
    hid.h \
    hid_tree.h

FORMS    += mainwindow.ui \
    hid_tree.ui

win32 {
    LIBS += -L$$PWD/lib_x32/
    LIBS += -lsetupapi -lhid
}

RESOURCES += \
    resources.qrc
