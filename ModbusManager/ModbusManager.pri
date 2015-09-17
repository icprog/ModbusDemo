INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
include(qextserialport/src/qextserialport.pri)
include(ZTools/ZTools.pri)
HEADERS += \
    ModbusManager/modbusmanager.h \
    ModbusManager/modbus.h

SOURCES += \
    ModbusManager/modbusmanager.cpp \
    ModbusManager/modbus.cpp
