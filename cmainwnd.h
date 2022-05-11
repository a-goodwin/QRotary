#ifndef CMAINWND_H
#define CMAINWND_H

#include <QDialog>
#include <QSerialPortInfo>
#include <QTimer>
#include <mapcontrol.h>
#include <osmmapadapter.h>
#include <maplayer.h>
#include <geometrylayer.h>
#include <imagepoint.h>
#include <geometry.h>
#define APP_VER "1.0.5"
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class CMainWnd; }
QT_END_NAMESPACE
using namespace qmapcontrol;

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

    void slSerialPortData();
    void slSerialPortString(QString st);
    void slGeometryClicked(Geometry *geometry, QPoint point);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void log(QString st);
    void saveConf();
    void LoadConf();

    Ui::CMainWnd *ui;

    QSettings *set;
    QSerialPort *ser = nullptr;
    QSerialPortInfo info;
    int savedPos = -1;

    MapAdapter *mapadapter;
    Layer *mainlayer;
    ImagePoint *plane;
    GeometryLayer *planelayer;
    QString m_st="";
    float m_az  = 0;
    float m_el  = 0;

    QTimer sendTmr;

    QTcpSocket *sock;
};
#endif // CMAINWND_H
