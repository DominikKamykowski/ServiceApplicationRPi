#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientApi api("192.168.1.25:8000");
    std::vector<uint8_t> _data = api.getTemperature();
    std::cout<< std::string(_data.begin(),_data.end())<<std::endl;

}

MainWindow::~MainWindow()
{
    delete ui;
}

