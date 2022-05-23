INCLUDEPATH += mapviewer/src/
SUBDIRS += mapviewer/src
QT += network

win32 {
Debug:LIBS += -L$$_PRO_FILE_PWD_/mapviewer/bin -lqmapcontrol0d
Release:LIBS += -L$$_PRO_FILE_PWD_/mapviewer/bin -lqmapcontrol0
}
else {
Debug:LIBS += -L$$_PRO_FILE_PWD_/mapviewer/bin -lqmapcontrold
Release:LIBS += -L$$_PRO_FILE_PWD_/mapviewer/bin -lqmapcontrol
}


