#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QObject>
#include "ipconnectiondialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ip_dialog = new IpConnectionDialog(this);
    connect(ip_dialog,&IpConnectionDialog::accepted,this,&MainWindow::setConnection_string);

    uiSettings();
    readStyleFiles();
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
    setConnection_string();
    this->ui->pbConnect->setStyleSheet("color:red");

    setLabelState(this->ui->lbHDMI0Status,false);
    setLabelState(this->ui->lbHDMI1Status,false);
    setLabelState(this->ui->lbSecondaryLCDStatus,false);
    setLabelState(this->ui->lbMainLCDStatus,false);
    setLabelState(this->ui->lbCompositeStatus,false);
    setProgressBarStyle(this->ui->pbCpuUsage,true);
    setProgressBarStyle(this->ui->pbUsedDiskSpace,true);

}

void MainWindow::fixGPSUiChange(ClientApi::GPS_Fix_t _fix)
{
    //FIX quality
    if (fixQualityMap->find(_fix.fix_quality) != fixQualityMap->end())
    {
        this->ui->lbFixQualityValue->setText(fixQualityMap->at(_fix.fix_quality));
    }
    else
    {
        this->ui->lbFixQualityValue->setText("---");
    }

    //FIX type
    if (fixTypeMap->find(_fix.fix_quality_3d) != fixTypeMap->end())
    {
        this->ui->lbFixTypeValue->setText(fixTypeMap->at(_fix.fix_quality_3d));
    }
    else
    {
        this->ui->lbFixTypeValue->setText("---");
    }
}

void MainWindow::setLabelState(QLabel * label, bool state)
{
    if(state)
    {
        label->setText("Active");
        label->setStyleSheet(css_label_screen_ON);
    }
    else
    {
        label->setText("Inactive");
        label->setStyleSheet(css_label_screen_OFF);
    }
}

void MainWindow::setConnection_string()
{
    connection_string = ip_dialog->getConnectionString();
    this->ui->lbConnectionString->setText(connection_string);
}

void MainWindow::readStyleFiles()
{
    style_names.append(&css_progressBar_bad_data);
    style_names.append(&css_progressBar_good_data);
    style_names.append(&css_label_screen_ON);
    style_names.append(&css_label_screen_OFF);

    QFile file;
    for (int var = 0; var < style_files.count(); ++var) {
        file.setFileName(style_files.at(var));
        file.open(QFile::ReadOnly);
        *style_names.at(var) = QLatin1String(file.readAll());
        file.close();
    }
}

void MainWindow::setProgressBarStyle(QProgressBar * const progressBar, bool state)
{
    if(state) progressBar->setStyleSheet(css_progressBar_good_data);
    else progressBar->setStyleSheet(css_progressBar_bad_data);
}

void MainWindow::ClientApi_onCpuTemperatureChanged(float cpu_temperature)
{ this->ui->dsbCpuTemp->setValue(static_cast<double>(cpu_temperature)); }

void MainWindow::ClientApi_onCpuVoltsChanged(float cpu_volts)
{ this->ui->dsbCpuVolts->setValue(static_cast<double>(cpu_volts)); }

void MainWindow::ClientApi_onCpuUsageChanged(float cpu_usage)
{
    this->ui->pbCpuUsage->setValue(static_cast<int>(cpu_usage));
    if(cpu_usage > 80) setProgressBarStyle(this->ui->pbCpuUsage,false);
    else setProgressBarStyle(this->ui->pbCpuUsage,true);
}

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
    if(_disk.percent > 90) setProgressBarStyle(this->ui->pbUsedDiskSpace,false);
    else setProgressBarStyle(this->ui->pbUsedDiskSpace,true);
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
        api = new ClientApi(connection_string.toStdString());
        QObject::connect(this->ui->pbGetAllBME280, &QPushButton::clicked,
                         api, &ClientApi::getBme280);


        QObject::connect(this->ui->pbGetGPSData, &QPushButton::clicked,
                         api, &ClientApi::getGPS);

        api->addEventListener(this);
        this->ui->tabWidgeMain->setEnabled(true);
        this->ui->pbConnectConfigure->setEnabled(false);
        this->ui->pbConnect->setStyleSheet("color:green");
    }
    else
    {
        this->ui->tabWidgeMain->setEnabled(false);
        this->ui->cbAutoRefresh->setCheckState(Qt::CheckState::Unchecked);
        this->ui->pbDataRefresh->setEnabled(true);
        api->removeEventListener(this);
        this->ui->pbConnectConfigure->setEnabled(true);
        this->ui->pbConnect->setStyleSheet("color:red");

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

void MainWindow::on_pbConnectConfigure_clicked()
{
    ip_dialog->exec();
}

