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
    DBG_MS(2, "INIT");
    QCoreApplication::setApplicationName("QRotaryCtl");
    QCoreApplication::setOrganizationName("a-goodwin");
    ser = new QSerialPort(this);
    ui->setupUi(this);

    InitConf(set);

    ////// init ui elements from settings
    /// uart
    on_bRefresh_clicked();
    ui->cmSerial->setCurrentIndex(savedPos);

    // angles and other sliders

    connect(&sendTmr, &QTimer::timeout, this, &CMainWnd::slTimer);
    sendTmr.setInterval(ui->eSendTimer->value());
    sendTmr.setSingleShot(true);
    DBG_MS(2, "LoadConf");
    LoadConf();
    DBG_MS(2, "INIT DONE");
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
        if (savedUartName == info.portName()) savedPos = pos;
        pos++;
    }
}

void CMainWnd::on_cmSerial_currentIndexChanged(int index)
{
    if (ser->isOpen()) ser->close();
    ser->setPortName(ui->cmSerial->currentData().toString());
    set->setValue("cmSerial", ui->cmSerial->currentData().toString());
    if (!ser->open(QIODevice::WriteOnly)) {
        log(tr("cant open port %1").arg(ser->portName()));
        DBG_MS(2, tr("port %1 can not be opened!").arg(ser->portName()));
    }
    ser->setBaudRate(ui->eBaudRate->value());

}

void CMainWnd::slIfaceUpd()
{
    QString st;
    if (sender()->objectName()=="eElevation") {
        ui->sElevation->setValue(ui->eElevation->value());
    }

    if (sender()->objectName()=="eAzimuth") {
        ui->sAzimuth->setValue(ui->eAzimuth->value());
    }

    /*if (sender()->objectName() == "cmSerial") {
        set->setValue("cmSerial", ui->cmSerial->currentData().toString());
        return;
    }*/
    DBG_MS(3, tr("slIfaceUpd %1").arg(sender()->objectName()));
    if (ui->eAzimuth->value()<ui->eAzimuthMin->value()) ui->eAzimuth->setValue(ui->eAzimuthMin->value());
    if (ui->eAzimuth->value()>ui->eAzimuthMax->value()) ui->eAzimuth->setValue(ui->eAzimuthMax->value());

    if (ui->eElevation->value()<ui->eElevationMin->value()) ui->eElevation->setValue(ui->eElevationMin->value());
    if (ui->eElevation->value()>ui->eElevationMax->value()) ui->eElevation->setValue(ui->eElevationMax->value());


    st = ui->eStrTemplate->text().arg(ui->eAzimuth->value()).arg(ui->eElevation->value());
    DBG_MS(2, st);
    ui->eSendString->setText(st);

    //saveConf();

    sendTmr.start(ui->eSendTimer->value());
}

void CMainWnd::on_bManualSend_clicked()
{
    slSendString();
}

void CMainWnd::saveConf()
{
    Ui2ConfInt(set, ui->eAzimuth);
    Ui2ConfInt(set, ui->eElevation);

    Ui2ConfInt(set, ui->eElevationMin);
    Ui2ConfInt(set, ui->eElevationMax);

    Ui2ConfInt(set, ui->eAzimuthMin);
    Ui2ConfInt(set, ui->eAzimuthMax);

    Ui2ConfInt(set, ui->eBaudRate);
    Ui2ConfInt(set, ui->eSendTimer);
    Ui2ConfStr(set, ui->eStrTemplate);
   // Ui2ConfStr(set, ui->eSendString);
    set->sync();
}

void CMainWnd::LoadConf()
{
    Conf2UiInt(set, ui->eAzimuth);
    //ui->sAzimuth->setValue(ui->eAzimuth->value());
    Conf2UiInt(set, ui->eElevation);
    //ui->sElevation->setValue(ui->eElevation->value());
    Conf2UiInt(set, ui->eSendTimer);
    Conf2UiInt(set, ui->eAzimuthMin);
    Conf2UiInt(set, ui->eAzimuthMax);
    Conf2UiInt(set, ui->eElevationMin);
    Conf2UiInt(set, ui->eElevationMax);
    Conf2UiStr(set, ui->eStrTemplate);
    //Conf2UiStr(set, ui->eSendString);
    Conf2UiInt(set, ui->eBaudRate);
}

void CMainWnd::slSendString()
{
    if (!ser->isOpen()) {
        log(tr("cant open port %1").arg(ser->portName()));
        DBG_MS(2, tr("port %1 can not be opened!").arg(ser->portName()));
        return;
    }
    QByteArray ba = ui->eSendString->text().toLocal8Bit().append('\n');
    ser->write(ba);
    ser->flush();
    log(tr("->(%1)").arg(ui->eSendString->text()));
}

void CMainWnd::slTimer()
{
    slSendString();
}


void CMainWnd::on_sAzimuth_sliderMoved(int position)
{
    DBG_MS(2, "sAzimuth");
    ui->eAzimuth->setValue(position);
}

void CMainWnd::on_sElevation_sliderMoved(int position)
{
    DBG_MS(2, "sElevation");
    ui->eElevation->setValue(position);
}

void CMainWnd::closeEvent(QCloseEvent *event)
{
    saveConf();
    ser->close();
    event->accept();
}

void CMainWnd::log(QString st)
{
    ui->eLog->appendPlainText(st);
}
