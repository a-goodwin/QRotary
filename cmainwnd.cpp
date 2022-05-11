#include "cmainwnd.h"
#include "ui_cmainwnd.h"

#include <QSettings>
#include "configutils.h"
#include "QSerialPort"

#define DBG_SRC_STR "MAIN"
#define DEBUG_LEVEL 4
#include "debug.h"

#include <QPoint>

CMainWnd::CMainWnd(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CMainWnd)
{
    DBG_MS(2, "INIT");
    QCoreApplication::setApplicationName("QRotaryCtl");
    QCoreApplication::setOrganizationName("a-goodwin");
    ser = new QSerialPort(this);
    ui->setupUi(this);

    sock = new QTcpSocket(this);

    setWindowTitle(windowTitle()+" - "+APP_VER);

    InitConf(set, "");

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

    ui->mc->showScale(true);
    ui->mc->enableMouseWheelEvents();
    //mapadapter = new OSMMapAdapter();
    //mainlayer = new MapLayer("OSM-online Map Layer", mapadapter);
    //ui->mc->addLayer(mainlayer);
    //ui->mc->setZoom(4);
    //ui->mc->showCrosshairs(true);

    //QPointF c = QPointF(39.85, 57.54);
    ui->mc->resize(ui->tabGeoMap->size());

    plane = new ImagePoint(ui->eLocalLon->value(), ui->eLocalLat->value() , QString("G:\\WORK\\QT\\QRotary\\1.jpg") /*"qrc:/1.png")*/);
    mapadapter = new OSMMapAdapter();

    mainlayer = new MapLayer("maplayer", mapadapter);
    planelayer = new GeometryLayer("geometry",  mapadapter);
    planelayer->addGeometry(plane);
    ui->mc->addLayer(mainlayer);
    ui->mc->addLayer(planelayer);
    QPointF c(ui->eLocalLon->value(), ui->eLocalLat->value());
    ui->mc->setView(c);
    plane->setCoordinate(c);
    ui->mc->setZoom(16);
    ui->mc->showCrosshairs(true);
    plane->setVisible(true);
    on_cmSerial_currentIndexChanged(0);
    connect(planelayer, &GeometryLayer::geometryClicked, this, &CMainWnd::slGeometryClicked);
}

CMainWnd::~CMainWnd()
{
    set->deleteLater();
    ser->close();
    ser->deleteLater();
    sock->deleteLater();
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
    Q_UNUSED(index);

    if (ser) {
        if (ser->isOpen()) {
            disconnect(ser,0,0,0);
            ser->close();
        }
    }

    ser->setPortName(ui->cmSerial->currentData().toString());
    set->setValue("cmSerial", ui->cmSerial->currentData().toString());
    if (!ser->open(QIODevice::WriteOnly)) {
        log(tr("can't open port %1").arg(ser->portName()));
        DBG_MS(2, tr("port %1 can not be opened!").arg(ser->portName()));
    } else {
        connect(ser, &QSerialPort::readyRead, this, &CMainWnd::slSerialPortData);
        log(tr("Port %1 opened").arg(ser->portName()));
        DBG_MS(2, tr("Port %1 opened!").arg(ser->portName()));
    };
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
    if (sender()->objectName() == "eBaudRate") {
        on_cmSerial_currentIndexChanged(0);
        return;
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

void CMainWnd::slSerialPortData()
{
    //if (!ser) return;
    QByteArray data = ser->readAll();
    log(tr("<=\"%1\"").arg(QString(data)));
    // data processing
    foreach( char c, data) {
        switch (c) {
        case 0x0a: // CR
            if (m_st.length()>0) slSerialPortString(m_st);
            m_st.clear();
            break;
        case 0x0d:
            if (m_st.length()>0) slSerialPortString(m_st);
            m_st.clear();
            break;
        default: //symbols
            m_st.append(c);
            break;
        }
    }
}

void CMainWnd::slSerialPortString(QString st)
{
    bool ok = false;
    log(tr("<-\"%1\"").arg(st));
    // parse string
    QString st1, st2;
    st1 = st.left(st.indexOf(' ')-1);
    st2 = st.mid(st.indexOf(' ')+1);
    float az = st1.toFloat(&ok);
    if (ok) m_az = az;
    float el = st2.toFloat(&ok);
    if (ok) m_el = el;

    ui->sAzView->setValue(m_az);
    ui->sElView->setValue(m_el);
}

void CMainWnd::slGeometryClicked(Geometry *geometry, QPoint point)
{
    DBG(1) << "geometry clicked" << point;
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
