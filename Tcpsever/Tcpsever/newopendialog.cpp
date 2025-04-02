#include "newopendialog.h"
#include "ui_newopendialog.h"
#include <QMessageBox>

NewopenDialog::NewopenDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewopenDialog)
{
    ui->setupUi(this);
}

NewopenDialog::~NewopenDialog()
{
    delete ui;
}




void NewopenDialog::on_buttonBox_accepted()
{
    qDebug()<<"sure";
    QString userid = ui->UseridEdit->text();
    QString passwd = ui->PasswordEdit->text();
    if(userid == nullptr || passwd == nullptr)
    {
        QMessageBox::information(this,"开户提醒", "用户名或密码为空!");
    }
    else
    {
        emit sendusers(userid, passwd);
        qDebug()<<"newopen_send";
    }
}


void NewopenDialog::on_buttonBox_rejected()
{
    qDebug()<<"return";
    return;
}

