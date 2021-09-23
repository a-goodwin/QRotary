#include "cmainwnd.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWnd w;
    w.show();
    return a.exec();
}
