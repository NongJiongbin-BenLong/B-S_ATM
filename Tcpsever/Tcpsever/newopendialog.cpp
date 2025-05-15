#include "newopendialog.h"
#include "ui_newopendialog.h"


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


void NewopenDialog::on_buttonBox_accepted() //点击“确定”按钮时提交开户信息
{
    qDebug()<<"sure";
    QString userid = ui->UseridEdit->text();
    QString passwd = ui->PasswordEdit->text();
    if(userid == nullptr || passwd == nullptr)
    {   //判断用户名和密码是否为空
        QMessageBox::information(this,"开户提醒", "用户名或密码为空!");
    }
    else
    {   //发送信号，传递用户名和密码
        emit sendusers(userid, passwd);
        qDebug()<<"newopen_send";
    }
}

void NewopenDialog::on_buttonBox_rejected() //点击“取消”按钮时返回
{
    qDebug()<<"return";
    return;
}

