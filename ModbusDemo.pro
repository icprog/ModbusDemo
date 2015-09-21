#-------------------------------------------------
#
# Project created by QtCreator 2015-09-15T16:24:18
#
#-------------------------------------------------

QT       += core gui


include(ModbusManager/ModbusManager.pri)
TARGET = ModbusDemo
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui




DEFINES += $$(ARCH)
contains( DEFINES,arm ) {
    DEFINES += ARM
}
DEFINES += ARCH
!contains( DEFINES,arm ) {
    DEFINES += X86
}




contains( DEFINES, ARM ) {


    mytarget.commands = scp ./${TARGET} root@192.168.1.243:/
}


mytarget.target = a

mytarget.depends = all



QMAKE_EXTRA_TARGETS += mytarget
