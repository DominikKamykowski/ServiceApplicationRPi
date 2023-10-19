#ifndef IPCONNECTIONDIALOG_H
#define IPCONNECTIONDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>

class IpConnectionDialog : public QDialog
{
public:
    IpConnectionDialog(QWidget *parent = nullptr);
    virtual ~IpConnectionDialog();

    QString getIp() const;
    int getPort() const;
    QString getConnectionString() const;


private:
    QFormLayout * mainLayout = nullptr;
    QPushButton * okBtn = nullptr;

    QRegularExpression IpRegex{"^(([0-9]|[1-9][0-9]|1[0-9]\{2}|2[0-4][0-9]|25[0-4])\\.){3}([1-9]|[1-9][0-9]|1[0-9]\{2}|2[0-4][0-9]|25[0-4])$"}; //RegEx na adres IP
    QValidator * ipValidator = nullptr;

    //--- SpinBox Port
#define MIN_PORT_VALUE 1024
#define MAX_PORT_VALUE 65535
    QSpinBox * sb_port = nullptr;
    void sb_portInitialize(int min, int max, int default_value);

    //--- LineEdit IP
    QLineEdit * le_ip = nullptr;
    bool checkValidate();


    QString ip = "192.168.1.25";
    int port = 8000;

    void accept() override;


    //--- Style
    void readStyleFiles();
    QStringList style_files{":/css/style/LineEditBad.qss",
                            ":/css/style/LineEditGood.qss"};
    QList<QString*> style_names;

    QString css_LineEdit_bad_data;
    QString css_LineEdit_good_data;
};

#endif // IPCONNECTIONDIALOG_H
