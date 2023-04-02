#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientApi api("192.168.1.25:8000");
    std::vector<float> data = api.getDiskUsage();
    std::cout<<data.at(3)<<std::endl;

}

MainWindow::~MainWindow()
{
    delete ui;
}

