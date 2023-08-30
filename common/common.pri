
include(beep/beep.pri)
include(charts/charts.pri)
include(keyboard/keyboard.pri)
include(customdialplot/customdialplot.pri)
include(modbus/modbusbase.pri)

INCLUDEPATH += $$PWD

HEADERS += \	
    $$PWD/common.h \
    $$PWD/msgbox.h \
    $$PWD/sysconfigfile.h \
    $$PWD/datapacket.h \
    $$PWD/qcustomplot.h \
    $$PWD/myprogressbar.h \
    $$PWD/comtablewid.h
    #$$PWD/permissions.h
	
SOURCES += \	
    $$PWD/common.cpp \
    $$PWD/msgbox.cpp \
    $$PWD/sysconfigfile.cpp \
    $$PWD/datapacket.cpp \
    $$PWD/qcustomplot.cpp \
    $$PWD/myprogressbar.cpp \
    $$PWD/comtablewid.cpp
    #$$PWD/permissions.cpp

FORMS += \
    $$PWD/comtablewid.ui




