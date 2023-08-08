#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientapi.h"
#include <iostream>
#include <QFile>
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
    void on_pbGetGPSData_clicked();
    void on_cbGpsAuto_clicked();

private:
    ClientApi *api = nullptr;

    std::map<ClientApi::FIX_QUALITY, QString> * fixQualityMap = nullptr;

    Ui::MainWindow *ui;
    void uiSettings();

    void fixGPSUiChange(ClientApi::GPS_Fix_t);


    // ClientApiEventListener interface
public:
    void ClientApi_onCpuTemperatureChanged(float);
    void ClientApi_onCpuVoltsChanged(float);
    void ClientApi_onCpuUsageChanged(float);

    void ClientApi_onServerTimeChanged(std::string);

    void ClientApi_onErrorMessageOccured(std::string);
    void ClientApi_onJsonParseError(std::string);
    void ClientApi_onJsonObjectNull(std::string);
\
    void ClientApi_onBME280TemperatureChanged(float);
    void ClientApi_onBME280HumidityChanged(float);
    void ClientApi_onBME280PressureChanged(float);

    void ClientApi_onRawJSON(QJsonDocument);

    void ClientApi_onGPSDataChanged(ClientApi::GPS_t);

    void ClientApi_onClocksChanged(ClientApi::Clocks_t);
    void ClientApi_onDisplayChanged(ClientApi::Displays_t);
    void ClientApi_onLoadAvgChanged(ClientApi::LoadAvg_t);
    void ClientApi_onVirtualMemoryChanged(ClientApi::VirtualMemory_t);
    void ClientApi_onDiskDataChanged(ClientApi::DiskUsage_t);

};
#endif // MAINWINDOW_H
