#-------------------------------------------------
#
# Project created by QtCreator 2015-09-15T16:24:18
#
#-------------------------------------------------

QT       += core gui

include(qextserialport/src/qextserialport.pri)
include(ModbusManager/ModbusManager.pri)
include(ZTools/ZTools.pri)
TARGET = ModbusDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui




DEFINES += $$(ARCH)
contains( DEFINES,arm ) {
    mytarget.commands = scp ./${TARGET} root@192.168.1.30:/
}

mytarget.target = a

mytarget.depends = all



QMAKE_EXTRA_TARGETS += mytarget
