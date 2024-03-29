#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientapi.h"
#include <iostream>
#include <QFile>
#include <QLabel>
#include "ipconnectiondialog.h"
#include <QProgressBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow, ClientApiEventListener
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbDataRefresh_clicked();
    void on_cbAutoRefresh_clicked();
    void on_pbConnect_clicked();
    void on_cbAutoGetBME280_clicked();
    void on_pbStopAllTimers_clicked();
    void on_pbClearDebugConsole_clicked();
    void on_cbGpsAuto_clicked();
    void on_pbConnectConfigure_clicked();

private:
    ClientApi *api = nullptr;

    std::map<ClientApi::FIX_QUALITY, QString> * fixQualityMap = new std::map<ClientApi::FIX_QUALITY, QString> {
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
    std::map<ClientApi::FIX_TYPE, QString> * fixTypeMap = new std::map<ClientApi::FIX_TYPE, QString> {
        {ClientApi::FIX_TYPE::NO_FIX_TYPE, "No fix type"},
        {ClientApi::FIX_TYPE::FIX_2D, "2D fix"},
        {ClientApi::FIX_TYPE::FIX_3D, "3D fix"},
    };

    Ui::MainWindow *ui;
    void uiSettings();

    void fixGPSUiChange(ClientApi::GPS_Fix_t);

    void setLabelState(QLabel*, bool);

    IpConnectionDialog * ip_dialog = nullptr;
    QString connection_string = "";
    void setConnection_string();


    //--- Style
    void readStyleFiles();
    void setProgressBarStyle(QProgressBar * const, bool);
    QStringList style_files{":/css/style/ProgressBarBad.css",
                            ":/css/style/ProgressBarGood.css",
                            ":/css/style/LabelScreenActive.css",
                            ":/css/style/LabelScreenOFF.css"};
    QList<QString*> style_names;
    QString css_progressBar_bad_data;
    QString css_progressBar_good_data;
    QString css_label_screen_ON;
    QString css_label_screen_OFF;

    // ClientApiEventListener interface
public:
    void ClientApi_onCpuTemperatureChanged(float) override;
    void ClientApi_onCpuVoltsChanged(float)override;
    void ClientApi_onCpuUsageChanged(float)override;
    void ClientApi_onServerTimeChanged(std::string)override;
    void ClientApi_onErrorMessageOccured(std::string)override;
    void ClientApi_onJsonParseError(std::string)override;
    void ClientApi_onJsonObjectNull(std::string)override;
    void ClientApi_onRawJSON(QJsonDocument)override;
    void ClientApi_onGPSDataChanged(ClientApi::GPS_t)override;
    void ClientApi_onClocksChanged(ClientApi::Clocks_t)override;
    void ClientApi_onDisplayChanged(ClientApi::Displays_t)override;
    void ClientApi_onLoadAvgChanged(ClientApi::LoadAvg_t)override;
    void ClientApi_onVirtualMemoryChanged(ClientApi::VirtualMemory_t)override;
    void ClientApi_onDiskDataChanged(ClientApi::DiskUsage_t)override;
    void ClientApi_onBME280TDataChanged(ClientApi::BME280_t)override;

};
#endif // MAINWINDOW_H
