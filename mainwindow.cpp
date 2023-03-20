#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientApi api;
    std::vector<uint8_t> _data = api.getTemperature();
    std::cout<< std::string(_data.begin(),_data.end())<<std::endl;

}

MainWindow::~MainWindow()
{
    delete ui;
}

