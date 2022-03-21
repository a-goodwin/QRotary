INCLUDEPATH += mapviewer/src/

win32 {
Debug:LIBS += -LG:/QT/QRotaryCtl/mapviewer/bin -lqmapcontrol0d
Release:LIBS += -LG:/QT/QRotaryCtl/mapviewer/bin -lqmapcontrol0
}
else {
Debug:LIBS += -LG:/QT/QRotaryCtl/mapviewer/bin -lqmapcontrold
Release:LIBS += -LG:/QT/QRotaryCtl/mapviewer/bin -lqmapcontrol
}

QT += network
