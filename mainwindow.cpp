#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    api = new ClientApi("192.168.1.25:8000");


//    ClientApi::DiskUsage_t data = api.getDiskUsage();
//    std::cout<<data.total<<std::endl;

}

MainWindow::~MainWindow()
{
    delete api;
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    ClientApi::Mainteance_t mainteance = api->getMainteance();
    std::cout<<mainteance.disk_usage.percent<<std::endl;
}

