#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    uiSettings();

    fixQualityMap = new std::map<ClientApi::FIX_QUALITY, QString> {
        {ClientApi::FIX_QUALITY::NO_FIX, "No fix"},
        {ClientApi::FIX_QUALITY::GPS_FIX, "GPS fix"},
        {ClientApi::FIX_QUALITY::DIFFERENTIAL_GPS_FIX, "Differential GPS fix"},
        {ClientApi::FIX_QUALITY::PPS_FIX, "PPS fix"},
        {ClientApi::FIX_QUALITY::REAL_TIME_KINEMATIC, "Real time kinematic"},
        {ClientApi::FIX_QUALITY::FLOAT_RTK, "Float RTK"},
        {ClientApi::FIX_QUALITY::ESTIMATED, "Estimated"},
        {ClientApi::FIX_QUALITY::MANUAL_INPUT_MODE, "Manual input mode"},
        {ClientApi::FIX_QUALITY::SIMULATION_MODE, "Simulation mode"}
    };
}

MainWindow::~MainWindow()
{
    delete fixQualityMap;
    if(api != nullptr)
    {
        delete api;
    }
    delete ui;
}

void MainWindow::on_pbDataRefresh_clicked()
{
    api->getMainteance();
}

void MainWindow::uiSettings()
{
    this->ui->pbConnect->setCheckable(true);
    this->ui->tabWidgeMain->setEnabled(false);
}

void MainWindow::fixGPSUiChange(ClientApi::GPS_Fix_t _fix)
{
    //FIX quality
    switch(_fix.fix_quality)
    {
    case ClientApi::FIX_QUALITY::NO_FIX:
    {
        this->ui->lbFixQualityValue->setText("No fix");
        break;
    }
    case ClientApi::FIX_QUALITY::GPS_FIX:
    {
        this->ui->lbFixQualityValue->setText("GPS fix");
        break;
    }
    case ClientApi::FIX_QUALITY::DIFFERENTIAL_GPS_FIX:
    {
        this->ui->lbFixQualityValue->setText("Differential GPS fix");
        break;
    }
    case ClientApi::FIX_QUALITY::PPS_FIX:
    {
        this->ui->lbFixQualityValue->setText("PPS fix");
        break;
    }
    case ClientApi::FIX_QUALITY::REAL_TIME_KINEMATIC:
    {
        this->ui->lbFixQualityValue->setText("Real time kinematic");
        break;
    }
    case ClientApi::FIX_QUALITY::FLOAT_RTK:
    {
        this->ui->lbFixQualityValue->setText("Float RTK");
        break;
    }
    case ClientApi::FIX_QUALITY::ESTIMATED:
    {
        this->ui->lbFixQualityValue->setText("Estimated");
        break;
    }
    case ClientApi::FIX_QUALITY::MANUAL_INPUT_MODE:
    {
        this->ui->lbFixQualityValue->setText("Manual input mode");
        break;
    }
    case ClientApi::FIX_QUALITY::SIMULATION_MODE:
    {
        this->ui->lbFixQualityValue->setText("Simulation mode");
        break;
    }
    default:
    {
        break;
    }
    }

    //Proponowane
//    if (fixQualityMap->find(_fix.fix_quality) != fixQualityMap->end())
//    {
//        this->ui->lbFixQualityValue->setText(fixQualityMap->at(_fix.fix_quality));
//    }
//    else
//    {
//        this->ui->lbFixQualityValue->setText("Nieznany typ");
//    }

    //FIX type
    switch(_fix.fix_quality_3d)
    {
    case ClientApi::FIX_TYPE::NO_FIX_TYPE:
    {

        break;
    }
    case ClientApi::FIX_TYPE::FIX_2D:
    {

        break;
    }
    case ClientApi::FIX_TYPE::FIX_3D:
    {

        break;
    }
    default:
    {
        break;
    }
    }
}

void MainWindow::setLabelState(QLabel * label, bool state)
{
    if(state)
    {
        label->setText("Active");
//        label->setStyleSheet("{color: #00FF00}");
    }
    else
    {
        label->setText("Inactive");
//        label->setStyleSheet("{color: #FF0000}");
    }
}

void MainWindow::ClientApi_onCpuTemperatureChanged(float cpu_temperature)
{ this->ui->dsbCpuTemp->setValue(static_cast<double>(cpu_temperature)); }

void MainWindow::ClientApi_onCpuVoltsChanged(float cpu_volts)
{ this->ui->dsbCpuVolts->setValue(static_cast<double>(cpu_volts)); }

void MainWindow::ClientApi_onCpuUsageChanged(float cpu_usage)
{ this->ui->pbCpuUsage->setValue(static_cast<int>(cpu_usage)); }

void MainWindow::ClientApi_onClocksChanged(ClientApi::Clocks_t _clocks)
{
    this->ui->dsbArmCores->setValue(_clocks.ARM_cores/1000000.0f);
    this->ui->dsbVC4->setValue(_clocks.VC4/1000000.0f);
    this->ui->dsbISP->setValue(_clocks.ISP/1000000.0f);
    this->ui->dspBlock3D->setValue(_clocks.block_3D/1000000.0f);
    this->ui->dsbUART->setValue(_clocks.UART/1000000.0f);
    this->ui->dsbPWM->setValue(_clocks.PWM/1000000.0f);
    this->ui->dsbEMMC->setValue(_clocks.EMMC/1000000.0f);
    this->ui->dsbPixel->setValue(_clocks.Pixel/1000000.0f);
    this->ui->dsbAVE->setValue(_clocks.AVE/1000000.0f);
    this->ui->dsbHDMI->setValue(_clocks.HDMI/1000000.0f);
    this->ui->dsbDPI->setValue(_clocks.DPI/1000000.0f);
}

void MainWindow::ClientApi_onDisplayChanged(ClientApi::Displays_t _displays)
{
    setLabelState(this->ui->lbMainLCDStatus,_displays.MainLCD);
    setLabelState(this->ui->lbSecondaryLCDStatus,_displays.SecondaryLCD);
    setLabelState(this->ui->lbHDMI0Status,_displays.HDMI0);
    setLabelState(this->ui->lbCompositeStatus,_displays.Composite);
    setLabelState(this->ui->lbHDMI1Status,_displays.HDMI1);
}

void MainWindow::ClientApi_onLoadAvgChanged(ClientApi::LoadAvg_t _load)
{
    this->ui->dsbLoadL1->setValue(static_cast<double>(_load.L1));
    this->ui->dsbLoadL2->setValue(static_cast<double>(_load.L2));
    this->ui->dsbLoadL3->setValue(static_cast<double>(_load.L3));
}

void MainWindow::ClientApi_onVirtualMemoryChanged(ClientApi::VirtualMemory_t _virt)
{
    this->ui->dsbTotalVM->setValue(_virt.total/(pow(1024,3)));
    this->ui->dsbAvailableVM->setValue(_virt.available/(pow(1024,3)));
    this->ui->dsbUsedVM->setValue(_virt.used/(pow(1024,3)));
    this->ui->dsbFreeVM->setValue(_virt.free/(pow(1024,3)));
    this->ui->dsbActiveVM->setValue(_virt.active/(pow(1024,3)));
    this->ui->dsbInactiveVM->setValue(_virt.inactive/(pow(1024,3)));
    this->ui->dsbBuffersVM->setValue(_virt.buffers/(pow(1024,3)));
    this->ui->dsbCachedVM->setValue(_virt.cached/(pow(1024,3)));
    this->ui->dsbSharedVM->setValue(_virt.shared/(pow(1024,3)));
    this->ui->dsbSlabVM->setValue(_virt.slab/(pow(1024,3)));
    this->ui->dsbWiredVM->setValue(_virt.wired/(pow(1024,3)));
}

void MainWindow::ClientApi_onDiskDataChanged(ClientApi::DiskUsage_t _disk)
{
    this->ui->dsbTotalDiskSpace->setValue(_disk.total/(pow(1024,3)));
    this->ui->dsbUsedDiskSpace->setValue(_disk.used/(pow(1024,3)));
    this->ui->dsbFreeDiskSpace->setValue(_disk.free/(pow(1024,3)));
    this->ui->pbUsedDiskSpace->setValue(static_cast<int>(_disk.percent));
}

void MainWindow::ClientApi_onBME280TDataChanged(ClientApi::BME280_t bme)
{
    this->ui->dsbExternalTemp->setValue(bme.temperature);
    this->ui->dsbHumidity->setValue(bme.humidity);
    this->ui->dsbPressure->setValue(bme.pressure);
}

void MainWindow::ClientApi_onServerTimeChanged(std::string m_time)
{
    this->ui->lbServerTime->setText(QString::fromStdString(m_time));
}

void MainWindow::ClientApi_onErrorMessageOccured(std::string message)
{
    this->ui->statusbar->showMessage(QString::fromStdString("Info: " + message),500);
}

void MainWindow::ClientApi_onJsonParseError(std::string message)
{
    this->ui->statusbar->showMessage(QString::fromStdString("Json parse data error: " + message),500);
}

void MainWindow::ClientApi_onJsonObjectNull(std::string message)
{
    this->ui->statusbar->showMessage(QString::fromStdString("Json object null in: " + message),500);
}

void MainWindow::ClientApi_onRawJSON(QJsonDocument doc)
{

    if(this->ui->cbDebugConsoleEnable->checkState())
    {
        this->ui->teDebug->append("------------- New data. Timestamp: "+QDateTime::currentDateTimeUtc().toString());
        this->ui->teDebug->append(doc.toJson());
    }

}

void MainWindow::ClientApi_onGPSDataChanged(ClientApi::GPS_t _gps)
{
    this->ui->dsbLongtitude->setValue(_gps.coordinates.longitude);
    this->ui->dsbLatitude->setValue(_gps.coordinates.latitude);
    this->ui->dsbAltitude->setValue(_gps.coordinates.altitude);
    this->ui->lbTimestamp->setText(QString::fromStdString(_gps.timestamp));

    this->ui->dsbPreciseLatitude->setValue(_gps.precise.latitude);
    this->ui->dsbPreciseLongitude->setValue(_gps.precise.longitude);

    this->ui->dsbGPSSpeed->setValue(_gps.speed);

    fixGPSUiChange(_gps.fix);
}


void MainWindow::on_cbAutoRefresh_clicked()
{
    if(this->ui->cbAutoRefresh->isChecked())
    {
        this->ui->pbDataRefresh->setEnabled(false);
        this->ui->dsbRefreshPeriod->setEnabled(false);
        api->startTimer(ClientApi::TIMERS::MAINTEANCE,this->ui->dsbRefreshPeriod->value()*1000);
    }
    else
    {
        this->ui->pbDataRefresh->setEnabled(true);
        this->ui->dsbRefreshPeriod->setEnabled(true);
        api->stopTimer(ClientApi::TIMERS::MAINTEANCE);
    }
}

void MainWindow::on_pbConnect_clicked()
{
    if(this->ui->pbConnect->isChecked())
    {
        api = new ClientApi("192.168.1.25:8000"); //TODO
        QObject::connect(this->ui->pbGetAllBME280, &QPushButton::clicked,
                         api, &ClientApi::getBme280);


        QObject::connect(this->ui->pbGetGPSData, &QPushButton::clicked,
                         api, &ClientApi::getGPS);

        api->addEventListener(this);
        this->ui->tabWidgeMain->setEnabled(true);
    }
    else
    {
        this->ui->tabWidgeMain->setEnabled(false);
        this->ui->cbAutoRefresh->setCheckState(Qt::CheckState::Unchecked);
        this->ui->pbDataRefresh->setEnabled(true);
        api->removeEventListener(this);
        delete api;
    }
}

void MainWindow::on_cbAutoGetBME280_clicked()
{
    if(this->ui->cbAutoGetBME280->isChecked())
    {
        this->ui->pbGetAllBME280->setEnabled(false);
        this->ui->dsbAutoBmeDuration->setEnabled(false);
        api->startTimer(ClientApi::TIMERS::BME280,this->ui->dsbAutoBmeDuration->value()*1000);
    }
    else
    {
        this->ui->pbGetAllBME280->setEnabled(true);
        this->ui->dsbAutoBmeDuration->setEnabled(true);
        api->stopTimer(ClientApi::TIMERS::BME280);
    }
}


void MainWindow::on_pbStopAllTimers_clicked()
{
    this->ui->statusbar->showMessage("All timers stopped", 500);
    if(this->ui->cbAutoGetBME280->isChecked())
    {
        this->ui->cbAutoGetBME280->click();
    }
    if(this->ui->cbAutoRefresh->isChecked())
    {
        this->ui->cbAutoRefresh->click();
    }
    if(this->ui->cbGpsAuto->isChecked())
    {
        this->ui->cbGpsAuto->click();
    }
}


void MainWindow::on_pbClearDebugConsole_clicked()
{
    this->ui->teDebug->clear();
}


//void MainWindow::on_pbGetGPSData_clicked()
//{
//    api->getGPS();
//}


void MainWindow::on_pbGetGPSData_clicked()
{

}


void MainWindow::on_cbGpsAuto_clicked()
{
    if(this->ui->cbGpsAuto->isChecked())
    {
        this->ui->pbGetGPSData->setEnabled(false);
        this->ui->dsbGpsDuration->setEnabled(false);
        api->startTimer(ClientApi::TIMERS::GPS,this->ui->dsbGpsDuration->value()*1000);
    }
    else
    {
        this->ui->pbGetGPSData->setEnabled(true);
        this->ui->dsbGpsDuration->setEnabled(true);
        api->stopTimer(ClientApi::TIMERS::GPS);
    }
}

