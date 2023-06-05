#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientapi.h"
#include <iostream>

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

    void on_pushButton_clicked();

    void on_pbGetAllBME280_clicked();

private:
    ClientApi *api = nullptr;

    Ui::MainWindow *ui;
    void uiSettings();

    // ClientApiEventListener interface
public:
    void ClientApi_onCpuTemperatureChanged(float);
    void ClientApi_onCpuVoltsChanged(float);
    void ClientApi_onCpuUsageChanged(float);

    void ClientApi_onClockArmCoresChanged(uint32_t arm_cores_clock);
    void ClientApi_onClockVC4Changed(uint32_t vc4_clock);
    void ClientApi_onClockISPChanged(uint32_t isp_clock);
    void ClientApi_onClockBlock3DChanged(uint32_t block3d_clock);
    void ClientApi_onClockUARTChanged(uint32_t uart_clock);
    void ClientApi_onClockPWMChanged(uint32_t pwm_clock);
    void ClientApi_onClockEMMCChanged(uint32_t emmc_clock);
    void ClientApi_onClockPixelChanged(uint32_t pixel_clock);
    void ClientApi_onClockAVEChanged(uint32_t ave_clock);
    void ClientApi_onClockHDMIChanged(uint32_t hdmi_clock);
    void ClientApi_onClockDPIChanged(uint32_t dpi_clock);

    void ClientApi_onDisplaysMainLcdChanged(bool main_lcd);
    void ClientApi_onDisplaysSecondaryLcdChanged(bool secondary_lcd);
    void ClientApi_onDisplaysHDMI0Changed(bool hdmi0);
    void ClientApi_onDisplaysCompositeChanged(bool composite);
    void ClientApi_onDisplaysHDMI1Changed(bool hdmi1);

    void ClientApi_onLoadAvgL1Changed(float l1);
    void ClientApi_onLoadAvgL2Changed(float l2);
    void ClientApi_onLoadAvgL3Changed(float l3);

    void ClientApi_onVirtualMemoryTotalChanged(uint64_t vm_total);
    void ClientApi_onVirtualMemoryAvailableChanged(uint64_t vm_available);
    void ClientApi_onVirtualMemoryUsedChanged(uint64_t vm_used);
    void ClientApi_onVirtualMemoryFreeChanged(uint64_t vm_free);
    void ClientApi_onVirtualMemoryActiveChanged(uint64_t vm_active);
    void ClientApi_onVirtualMemoryInactiveChanged(uint64_t vm_inactive);
    void ClientApi_onVirtualMemoryBuffersChanged(uint64_t vm_buffers);
    void ClientApi_onVirtualMemoryCachedChanged(uint64_t vm_cached);
    void ClientApi_onVirtualMemorySharedChanged(uint64_t vm_shared);
    void ClientApi_onVirtualMemorySlabChanged(uint64_t vm_slab);
    void ClientApi_onVirtualMemoryWiredChanged(uint64_t vm_wired);

    void ClientApi_onDiskUsageTotalChanged(uint64_t disk_total);
    void ClientApi_onDiskUsageUsedChanged(uint64_t disk_used);
    void ClientApi_onDiskUsageFreeChanged(uint64_t disk_free);
    void ClientApi_onDiskUsagePercentChanged(float disk_percentage);

    void ClientApi_onServerTimeChanged(std::string);
};
#endif // MAINWINDOW_H
