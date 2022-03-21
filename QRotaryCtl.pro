
include(mapviewer/QMapControl.pri)

QT       += core gui network serialport svg location charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
greaterThan(QT_MAJOR_VERSION, 4):  cache()


CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

MOC_DIR = tmp
OBJECTS_DIR = obj

SOURCES += \
    main.cpp \
    cmainwnd.cpp

HEADERS += \
    ../myutils/configutils.h \
    ../myutils/debug.h \
    ../myutils/utils.h \
    cmainwnd.h

FORMS += \
    cmainwnd.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
