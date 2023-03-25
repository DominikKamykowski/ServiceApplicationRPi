#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ClientApi api("192.168.1.25:8000");
    ClientApi::Users_t data = api.getUsers();
//    std::cout<<"total: "<< data<<" free: "<< data.free<<std::endl;

}

MainWindow::~MainWindow()
{
    delete ui;
}

