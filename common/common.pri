
include(beep/beep.pri)
include(charts/charts.pri)
include(keyboard/keyboard.pri)
include(customdialplot/customdialplot.pri)
include(modbus/modbusbase.pri)

INCLUDEPATH += $$PWD

HEADERS +=\
    $$PWD/extendedlineedit.h\
    $$PWD/common.h \
    $$PWD/msgbox.h \
    $$PWD/mydoublespinbox.h \
    $$PWD/myspinbox.h \
    $$PWD/sysconfigfile.h \
    $$PWD/datapacket.h \
    $$PWD/qcustomplot.h \
    $$PWD/myprogressbar.h \
    $$PWD/comtablewid.h
    #$$PWD/permissions.h
	
SOURCES +=\
    $$PWD/extendedlineedit.cpp\
    $$PWD/common.cpp \
    $$PWD/msgbox.cpp \
    $$PWD/mydoublespinbox.cpp \
    $$PWD/myspinbox.cpp \
    $$PWD/sysconfigfile.cpp \
    $$PWD/datapacket.cpp \
    $$PWD/qcustomplot.cpp \
    $$PWD/myprogressbar.cpp \
    $$PWD/comtablewid.cpp
    #$$PWD/permissions.cpp

FORMS += \
    $$PWD/comtablewid.ui




