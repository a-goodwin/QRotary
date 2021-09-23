#ifndef CMAINWND_H
#define CMAINWND_H

#include <QDialog>
#include <QSerialPortInfo>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class CMainWnd; }
QT_END_NAMESPACE
class QSettings;
class QSerialPort;
class CMainWnd : public QDialog
{
    Q_OBJECT

public:
    CMainWnd(QWidget *parent = nullptr);
    ~CMainWnd();

private slots:
    void on_bRefresh_clicked();
    void on_cmSerial_currentIndexChanged(int index);
    void sendString();
private:
    Ui::CMainWnd *ui;

    QSettings *set;
    QSerialPort *ser;
    QSerialPortInfo info;
    int savedPos = -1;

    QTimer sendTmr;
};
#endif // CMAINWND_H
