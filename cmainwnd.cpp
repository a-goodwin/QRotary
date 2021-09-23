#include "cmainwnd.h"
#include "ui_cmainwnd.h"

#include <QSettings>
#include "../myutils/configutils.h"
#include "QSerialPort"

#define DBG_SRC_STR "MAIN"
#define DEBUG_LEVEL 4
#include "../myutils/debug.h"

CMainWnd::CMainWnd(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CMainWnd)
{
    QCoreApplication::setApplicationName("QRotaryCtl");
    QCoreApplication::setOrganizationName("Alex Goodwin");
    ser = new QSerialPort(this);
    ui->setupUi(this);

    InitConf(set);

    ////// init ui elements from settings
    /// uart
    on_bRefresh_clicked();
    ui->cmSerial->setCurrentIndex(savedPos);

    // angles and other sliders
    ui->sAzimuth->setValue(set->value("eAzimuth", ui->eAzimuth->value()).toInt());
    ui->eAzimuth->setValue(set->value("eAzimuth", ui->eAzimuth->value()).toInt());
    ui->sElevation->setValue(set->value("eElevation", ui->eElevation->value()).toInt());
    ui->eElevation->setValue(set->value("eElevation", ui->eElevation->value()).toInt());

    ui->eSendTimer->setValue(set->value("eSendTimer", ui->eSendTimer->value()).toInt()); // 50 ms

    ui->eStrTemplate->setText(set->value("eStrTemplate", ui->eStrTemplate->text()).toString());


    connect()
}

CMainWnd::~CMainWnd()
{
    set->deleteLater();
    ser->close();
    ser->deleteLater();
    delete ui;
}


void CMainWnd::on_bRefresh_clicked()
{
    QString savedUartName = set->value("cmSerial", "").toString();
    int pos = 0;
    savedPos = -1;
    foreach( info, QSerialPortInfo::availablePorts()) {
        ui->cmSerial->addItem(info.portName()+" - "+info.description(), info.portName());
        pos++;
        if (savedUartName == info.portName()) savedPos = pos;
    }
}


void CMainWnd::on_cmSerial_currentIndexChanged(int index)
{
    ser->setPortName(ui->cmSerial->currentData().toString());
}

void CMainWnd::sendString()
{

}

