#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <future>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    api = new ClientApi("192.168.1.25:8000");
    api->addEventListener(this);
}

MainWindow::~MainWindow()
{
    delete api;
    delete ui;
}

void MainWindow::on_pbDataRefresh_clicked()
{
    api->httpRequest();
}

void MainWindow::uiSettings()
{
    this->ui->dsbArmCores->setSuffix("MHz");
    this->ui->dsbVC4->setSuffix("MHz");
    this->ui->dsbISP->setSuffix("MHz");
}

void MainWindow::ClientApi_onCpuTemperatureChanged(float cpu_temperature)
{ this->ui->dsbCpuTemp->setValue(static_cast<double>(cpu_temperature)); }

void MainWindow::ClientApi_onCpuVoltsChanged(float cpu_volts)
{ this->ui->dsbCpuVolts->setValue(static_cast<double>(cpu_volts)); }

void MainWindow::ClientApi_onCpuUsageChanged(float cpu_usage)
{ this->ui->pbCpuUsage->setValue(static_cast<int>(cpu_usage)); }

void MainWindow::ClientApi_onClockArmCoresChanged(uint32_t arm_cores_clock)
{ this->ui->dsbArmCores->setValue(arm_cores_clock/1000000.0f); }

void MainWindow::ClientApi_onClockVC4Changed(uint32_t vc4_clock)
{ this->ui->dsbVC4->setValue(vc4_clock/1000000.0f); }

void MainWindow::ClientApi_onClockISPChanged(uint32_t isp_clock)
{ this->ui->dsbISP->setValue(isp_clock/1000000.0f); }

void MainWindow::ClientApi_onClockBlock3DChanged(uint32_t block3d_clock)
{ this->ui->dspBlock3D->setValue(block3d_clock/1000000.0f); }

void MainWindow::ClientApi_onClockUARTChanged(uint32_t uart_clock)
{ this->ui->dsbUART->setValue(uart_clock/1000000.0f); }

void MainWindow::ClientApi_onClockPWMChanged(uint32_t pwm_clock)
{ this->ui->dsbPWM->setValue(pwm_clock/1000000.0f); }

void MainWindow::ClientApi_onClockEMMCChanged(uint32_t emmc_clock)
{ this->ui->dsbEMMC->setValue(emmc_clock/1000000.0f); }

void MainWindow::ClientApi_onClockPixelChanged(uint32_t pixel_clock)
{ this->ui->dsbPixel->setValue(pixel_clock/1000000.0f); }

void MainWindow::ClientApi_onClockAVEChanged(uint32_t ave_clock)
{ this->ui->dsbAVE->setValue(ave_clock/1000000.0f); }

void MainWindow::ClientApi_onClockHDMIChanged(uint32_t hdmi_clock)
{ this->ui->dsbHDMI->setValue(hdmi_clock/1000000.0f); }

void MainWindow::ClientApi_onClockDPIChanged(uint32_t dpi_clock)
{ this->ui->dsbDPI->setValue(dpi_clock/1000000.0f); }

void MainWindow::ClientApi_onDisplaysMainLcdChanged(bool main_lcd)
{}

void MainWindow::ClientApi_onDisplaysSecondaryLcdChanged(bool secondary_lcd)
{}

void MainWindow::ClientApi_onDisplaysHDMI0Changed(bool hdmi0)
{}

void MainWindow::ClientApi_onDisplaysCompositeChanged(bool composite)
{}

void MainWindow::ClientApi_onDisplaysHDMI1Changed(bool hdmi1)
{}

void MainWindow::ClientApi_onLoadAvgL1Changed(float l1)
{ this->ui->dsbLoadL1->setValue(static_cast<double>(l1)); }

void MainWindow::ClientApi_onLoadAvgL2Changed(float l2)
{ this->ui->dsbLoadL2->setValue(static_cast<double>(l2)); }

void MainWindow::ClientApi_onLoadAvgL3Changed(float l3)
{ this->ui->dsbLoadL3->setValue(static_cast<double>(l3)); }

void MainWindow::ClientApi_onVirtualMemoryTotalChanged(uint64_t vm_total)
{ this->ui->dsbTotalVM->setValue(vm_total); }

void MainWindow::ClientApi_onVirtualMemoryAvailableChanged(uint64_t vm_available)
{ this->ui->dsbAvailableVM->setValue(vm_available); }

void MainWindow::ClientApi_onVirtualMemoryUsedChanged(uint64_t vm_used)
{ this->ui->dsbUsedVM->setValue(vm_used); }

void MainWindow::ClientApi_onVirtualMemoryFreeChanged(uint64_t vm_free)
{ this->ui->dsbFreeVM->setValue(vm_free); }

void MainWindow::ClientApi_onVirtualMemoryActiveChanged(uint64_t vm_active)
{ this->ui->dsbActiveVM->setValue(vm_active); }

void MainWindow::ClientApi_onVirtualMemoryInactiveChanged(uint64_t vm_inactive)
{ this->ui->dsbInactiveVM->setValue(vm_inactive); }

void MainWindow::ClientApi_onVirtualMemoryBuffersChanged(uint64_t vm_buffers)
{ this->ui->dsbBuffersVM->setValue(vm_buffers); }

void MainWindow::ClientApi_onVirtualMemoryCachedChanged(uint64_t vm_cached)
{ this->ui->dsbCachedVM->setValue(vm_cached); }

void MainWindow::ClientApi_onVirtualMemorySharedChanged(uint64_t vm_shared)
{ this->ui->dsbSharedVM->setValue(vm_shared); }

void MainWindow::ClientApi_onVirtualMemorySlabChanged(uint64_t vm_slab)
{ this->ui->dsbSlabVM->setValue(vm_slab); }

void MainWindow::ClientApi_onVirtualMemoryWiredChanged(uint64_t vm_wired)
{ this->ui->dsbWiredVM->setValue(vm_wired); }

void MainWindow::ClientApi_onDiskUsageTotalChanged(uint32_t disk_total)
{ this->ui->dsbTotalDiskSpace->setValue(disk_total); }

void MainWindow::ClientApi_onDiskUsageUsedChanged(uint32_t disk_used)
{ this->ui->dsbUsedDiskSpace->setValue(disk_used); }

void MainWindow::ClientApi_onDiskUsageFreeChanged(uint32_t disk_free)
{ this->ui->dsbFreeDiskSpace->setValue(disk_free); }

void MainWindow::ClientApi_onDiskUsagePercentChanged(float disk_percentage)
{ this->ui->pbUsedDiskSpace->setValue(static_cast<int>(disk_percentage)); }



void MainWindow::on_cbAutoRefresh_clicked()
{
    if(this->ui->cbAutoRefresh->isChecked())
    {
        this->ui->pbDataRefresh->setEnabled(false);
        this->ui->dsbRefreshPeriod->setEnabled(false);
        api->startTimer(this->ui->dsbRefreshPeriod->value());
    }
    else
    {
        this->ui->pbDataRefresh->setEnabled(true);
        this->ui->dsbRefreshPeriod->setEnabled(true);
        api->stopTimer();
    }
}
