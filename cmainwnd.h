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

    void slSendString();

    void slTimer(); // send timer slot

    void slIfaceUpd(); //any iface element update


    void on_bManualSend_clicked();

    void on_sAzimuth_sliderMoved(int position);

    void on_sElevation_sliderMoved(int position);

protected:
    void closeEvent(QCloseEvent* event) override;

private:

    void saveConf();
    void LoadConf();

    Ui::CMainWnd *ui;

    QSettings *set;
    QSerialPort *ser;
    QSerialPortInfo info;
    int savedPos = -1;

    QTimer sendTmr;
};
#endif // CMAINWND_H
