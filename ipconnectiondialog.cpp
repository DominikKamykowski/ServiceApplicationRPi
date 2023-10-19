#include "ipconnectiondialog.h"
#include <QLabel>
#include <QMessageBox>
#include <QFile>

IpConnectionDialog::IpConnectionDialog(QWidget *parent) : QDialog(parent)
{
    this->setWindowTitle("Connection");

    mainLayout = new QFormLayout(this);
    okBtn = new QPushButton("OK", this);
    le_ip = new QLineEdit("192.168.1.25", this);
    connect(le_ip,&QLineEdit::textChanged,this,&IpConnectionDialog::checkValidate);

    ipValidator = new QRegularExpressionValidator(IpRegex, this);

    le_ip->setValidator(ipValidator);
    sb_port = new QSpinBox(this);
    sb_portInitialize(MIN_PORT_VALUE,MAX_PORT_VALUE,8000);

    QLabel * info = new QLabel("Configure connection",this);
    info->setAlignment(Qt::AlignCenter);

    mainLayout->addRow(info);
    mainLayout->addRow("IP address:",le_ip);
    mainLayout->addRow("Port:",sb_port);
    mainLayout->addRow(okBtn);


    readStyleFiles();
    connect(okBtn,&QPushButton::clicked,this,&IpConnectionDialog::accept);
}

IpConnectionDialog::~IpConnectionDialog()
{

}

QString IpConnectionDialog::getIp() const
{ return ip; }

int IpConnectionDialog::getPort() const
{ return port; }

QString IpConnectionDialog::getConnectionString() const
{
    return ip + ":" + QString::number(port);
}

void IpConnectionDialog::sb_portInitialize(int min, int max, int default_value)
{
    sb_port->setMinimum(min);
    sb_port->setMaximum(max);
    sb_port->setValue(default_value);
}

bool IpConnectionDialog::checkValidate()
{
    if(le_ip->hasAcceptableInput())
    {
        le_ip->setStyleSheet(css_LineEdit_good_data);
        return true;
    }
    else
    {
        le_ip->setStyleSheet(css_LineEdit_bad_data);
        return false;
    }
}

void IpConnectionDialog::accept()
{
    if(checkValidate())
    {
        this->ip = le_ip->text();
        this->port = sb_port->value();
        QDialog::accept();
    }
    else
    {
        QMessageBox::warning(this,"Warning","No valid input");
    }
}

void IpConnectionDialog::readStyleFiles()
{
    style_names.append(&css_LineEdit_bad_data);
    style_names.append(&css_LineEdit_good_data);

    QFile file;
    for (int var = 0; var < style_files.count(); ++var) {
        file.setFileName(style_files.at(var));
        file.open(QFile::ReadOnly);
        *style_names.at(var) = QLatin1String(file.readAll());
        file.close();
    }

}
