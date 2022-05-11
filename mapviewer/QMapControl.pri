INCLUDEPATH += mapviewer/src/

win32 {
Debug:LIBS += -LG:/WORK/QT/QRotary/mapviewer/bin -lqmapcontrol0d
Release:LIBS += -LG:/WORK/QT/QRotary/mapviewer/bin -lqmapcontrol0
}
else {
Debug:LIBS += -LG:/WORK/QT/QRotary/mapviewer/bin -lqmapcontrold
Release:LIBS += -LG:/WORK/QT/QRotary/mapviewer/bin -lqmapcontrol
}

QT += network

SUBDIRS += mapviewer/src
