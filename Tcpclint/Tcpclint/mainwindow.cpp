#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QRegExpValidator>
#include <QByteArray>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->page_wait);

    //127.0.0.1为本地回环地址，具体时需要正确ip地址
    ip = "10.242.228.41";
    S_port ="2525";
    port = S_port.toUShort();
    QEventLoop loop;

    //tcp创建
    tcp = new QTcpSocket(this);

    connect(tcp,&QTcpSocket::readyRead,this,[=]
            {
            });
    connect(tcp,&QTcpSocket::disconnected,this,[=]()
            {
                tcp->close();
                m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
                m_status->setProperty("status","disconnect");
            });

    connect(tcp,&QTcpSocket::connected,this,[=]
            {
                m_status->setPixmap(QPixmap(":/image/success.jpg").scaled(20,20));
                m_status->setProperty("status","connect");
            });
    //设置
    m_set = new QPushButton(this);
    m_set->setIconSize(QSize(20,20));
    m_set->setIcon(QIcon(":/image/set.png"));
    ui->statusbar->addWidget(m_set);
    connect(m_set,SIGNAL(clicked()),this,SLOT(on_m_set_clicked()));

    //状态栏
    m_status = new QLabel(this);
    m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
    ui->statusbar->addWidget(new QLabel("链接状态: "));
    m_status->setProperty("status","disconnect");
    ui->statusbar->addWidget(m_status);

    //sign
    ui->lineEdit_pwd1_s->setValidator(new QRegExpValidator(QRegExp("\\d{6}")));
    ui->lineEdit_pwd2_s->setValidator(new QRegExpValidator(QRegExp("\\d{6}")));
    ui->label_tip_s->setStyleSheet("color:red;");
    ui->label_userid_s->setProperty("color","black");

    //login
    ui->label_tip_l->setStyleSheet("color:red;");

    //password
    ui->lineEdit_pwd_p->setValidator(new QRegExpValidator(QRegExp("\\d{6}")));
    ui->label_tip_p->setStyleSheet("color:red;");

    //withdrawal
    ui->lineEdit_wd->setValidator(new QRegExpValidator(QRegExp("\\d{4}")));
    ui->label_tip_wd->setStyleSheet("color:red;");

    //deposit
    ui->lineEdit_d->setValidator(new QRegExpValidator(QRegExp("\\d{5}")));
    ui->label_tip_d->setStyleSheet("color:red;");

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::TCPDisconnect(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "您是否确定要退出ATM？",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply==QMessageBox::No){return;}
    else{
        QString msg="BYE";
        tcp->write(msg.toUtf8());
        tcp->close();
        on_pushButton_clear_l_clicked();
        on_pushButton_clear_s_clicked();
        on_pushButton_clear_p_clicked();
        on_pushButton_clear_wd_clicked();
        on_pushButton_clear_d_clicked();
        ui->stackedWidget->setCurrentWidget(ui->page_wait);
    }
}
void MainWindow::on_m_set_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("地址"),tr("请输入服务器所在ip地址："),QLineEdit::Normal,ip, &ok);
    QRegExp ipRegex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    if (ok && !text.isEmpty()&&ipRegex.exactMatch(text))
    {
        ip=text;
    }
    else{
        QMessageBox::warning(this, "Warning!", "输入不符合IPv4有效地址格式,服务器ip地址修改失败！");
    }
}
//page_wait
void MainWindow::on_pushButton_login_clicked()
{
    tcp->connectToHost(QHostAddress(ip),port);
    connect(tcp, &QTcpSocket::connected, &loop, &QEventLoop::quit);
    loop.exec();
    if(m_status->property("status")=="disconnect"){
        QMessageBox::information(this, "Info", "连接失败!");return;
    }else{
        on_pushButton_clear_l_clicked();
        ui->stackedWidget->setCurrentWidget(ui->page_login);
    }
}

void MainWindow::on_pushButton_sign_clicked()
{
    tcp->connectToHost(QHostAddress(ip),port);
    connect(tcp, &QTcpSocket::connected, &loop, &QEventLoop::quit);
    loop.exec();
    if(m_status->property("status")=="disconnect"){
        QMessageBox::information(this, "Info", "连接失败!");return;
    }else{
        on_pushButton_clear_s_clicked();
        ui->stackedWidget->setCurrentWidget(ui->page_sign);
    }
}
//page_sign
void MainWindow::on_pushButton_exit_s_clicked()
{
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_s_clicked()
{
    ui->lineEdit_userid_s->clear();
    ui->lineEdit_pwd1_s->clear();
    ui->lineEdit_pwd2_s->clear();
    ui->label_tip_s->clear();
    ui->label_userid_s->setStyleSheet("color:black;");
}

void MainWindow::on_pushButton_sign_s_clicked()
{
    QString userid = ui->lineEdit_userid_s->text();
    QString pwd1 = ui->lineEdit_pwd1_s->text();
    QString pwd2 = ui->lineEdit_pwd2_s->text();
    if(ui->label_userid_s->property("color")!="green"){qDebug()<<"Not Green";return;}
    else if(pwd1.isEmpty()){ui->label_tip_s->setText("请输入密码！");return;}
    else if(pwd2.isEmpty()){ui->label_tip_s->setText("请再次输入密码！");return;}
    else if(pwd1.length()!=6 || pwd2.length()!=6){
          ui->label_tip_s->setText("密码必须为6位数字！");
          return;
      }
    else if(pwd1!=pwd2){
          ui->label_tip_s->setText("两次密码不匹配！");
          return;
      }
     QString msg="SIGN "+userid+" "+pwd1;
     tcp->write(msg.toUtf8());
     connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
     loop.exec();
     QByteArray r = tcp->readAll();
     if (r.toStdString()=="525 OK!"){
         QMessageBox::information(this, "Info", "注册成功！");
         TCPDisconnect();
        ;}
     else{
         QMessageBox::information(this, "Info", "注册失败！请重新尝试！");
         return;
     }
}
void MainWindow::on_lineEdit_userid_s_editingFinished()
{
    QString userid = ui->lineEdit_userid_s->text();
    if(userid.isEmpty()){ui->label_tip_s->setText("卡号不能为空！");return;}
    else {
        QString msg="HELO "+userid;
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        if (r.toStdString()=="500 AUTH REQUIRED!"){
            ui->label_tip_s->setText("该卡号已注册！请登录或重新输入!");
            ui->label_userid_s->setStyleSheet("color:red;");
            ui->label_userid_s->setProperty("color","red");
            return;
           ;}
        else{
            ui->label_tip_s->setText("");
            ui->label_userid_s->setStyleSheet("color:green;");
            ui->label_userid_s->setProperty("color","green");
        }
    }
}

//page_login
void MainWindow::on_pushButton_exit_l_clicked()
{
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_l_clicked()
{
    ui->lineEdit_userid_l->clear();
    ui->label_tip_l->clear();
}

void MainWindow::on_pushButton_next_l_clicked()
{
    QString userid = ui->lineEdit_userid_l->text();
    if(userid.isEmpty()){ui->label_tip_l->setText("卡号不能为空！");return;}
    else {
        QString msg="HELO "+userid;
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        if (r.toStdString()=="500 AUTH REQUIRED!"){
            ui->label_tip_l->setText("");
            ui->label_tip_p->setText("");
            ui->stackedWidget->setCurrentWidget(ui->page_password);
           ;}
        else{
            ui->label_tip_l->setText("查询不到该卡号！请重新输入！");
            return;
        }
    }
}
//page_password
void MainWindow::on_pushButton_exit_p_clicked()
{
    TCPDisconnect();
}

void MainWindow::on_pushButton_clear_p_clicked()
{
    ui->lineEdit_pwd_p->clear();
    ui->label_tip_p->clear();
}

void MainWindow::on_pushButton_login_p_clicked()
{
    QString pwd = ui->lineEdit_pwd_p->text();
    if(pwd.isEmpty()){ui->label_tip_p->setText("请输入密码！");return;}
    else if(pwd.length()!=6){ui->label_tip_p->setText("密码必须为6位数字！");return;}
    else {
        QString msg="PASS "+pwd;
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        if (r.toStdString()=="525 OK!"){
            ui->label_tip_p->setText("");
            skipToFunction();
            }
        else{
            ui->label_tip_p->setText("密码输入错误！请重新尝试！");
            return;
        }
    }
}
//page_function
void MainWindow::skipToFunction(){
    ui->textBrowser_f->setText("亲爱的用户，欢迎使用ATM！");
    ui->pushButton_balance_f->setDisabled(false);
    ui->pushButton_balance_f->setVisible(true);
    ui->stackedWidget->setCurrentWidget(ui->page_function);
}
void MainWindow::on_pushButton_deposit_f_clicked()
{
    on_pushButton_clear_d_clicked();
    ui->stackedWidget->setCurrentWidget(ui->page_deposit);
}
void MainWindow::on_pushButton_withdrawal_f_clicked()
{
    on_pushButton_clear_wd_clicked();
    ui->stackedWidget->setCurrentWidget(ui->page_withdrawal);
}
void MainWindow::on_pushButton_balance_f_clicked()
{
        QString msg="BALA";
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        QStringList parts =QString::fromUtf8(r).split(" ");
        if (parts[0]=="AMNT"){
            ui->textBrowser_f->setText("您的余额是： "+parts[1]+" ￥。");
            ui->pushButton_balance_f->setDisabled(true);
            ui->pushButton_balance_f->setVisible(false);
           }
        else{
            ui->textBrowser_f->setText("余额获取失败！请重新尝试！");
            return;
        }

}

void MainWindow::on_pushButton_exit_f_clicked()
{
    TCPDisconnect();
}
//page_withdrawal
void MainWindow::on_pushButton_100_wd_clicked()
{
    ui->lineEdit_wd->setText("100");
}

void MainWindow::on_pushButton_200_wd_clicked()
{
    ui->lineEdit_wd->setText("200");
}

void MainWindow::on_pushButton_500_wd_clicked()
{
    ui->lineEdit_wd->setText("500");
}

void MainWindow::on_pushButton_800_wd_clicked()
{
    ui->lineEdit_wd->setText("800");
}

void MainWindow::on_pushButton_1000_wd_clicked()
{
    ui->lineEdit_wd->setText("1000");
}

void MainWindow::on_pushButton_2000_wd_clicked()
{
    ui->lineEdit_wd->setText("2000");
}

void MainWindow::on_pushButton_3000_wd_clicked()
{
    ui->lineEdit_wd->setText("3000");
}

void MainWindow::on_pushButton_5000_wd_clicked()
{
    ui->lineEdit_wd->setText("5000");
}

void MainWindow::on_pushButton_exit_wd_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "是否要放弃取款？",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply==QMessageBox::No){return;}
    else{skipToFunction();}
}

void MainWindow::on_pushButton_clear_wd_clicked()
{
    ui->lineEdit_wd->clear();
    ui->label_tip_wd->clear();
}

void MainWindow::on_pushButton_submit_wd_clicked()
{
    QString amnt = ui->lineEdit_wd->text();
    if(amnt.right(2)=="00" && (amnt.length()==3 || (amnt.length()==4 && amnt.left(1)<="5"))){
        QString msg="WDRA "+amnt;
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        if (r.toStdString()=="525 OK!"){
            QMessageBox::information(this, "Info", "取款成功！");
            skipToFunction();
           }
        else{
            QMessageBox::information(this, "Info", "余额不足！");
            return;
        }
    }else{
        ui->label_tip_wd->setText("输入金额必须为整百数！并且一次取款金额不能超过5000￥！");
        return;
    }
}
//page_deposit
void MainWindow::on_pushButton_100_d_clicked()
{
    ui->lineEdit_d->setText("100");
}

void MainWindow::on_pushButton_200_d_clicked()
{
    ui->lineEdit_d->setText("200");
}

void MainWindow::on_pushButton_500_d_clicked()
{
    ui->lineEdit_d->setText("500");
}

void MainWindow::on_pushButton_1000_d_clicked()
{
    ui->lineEdit_d->setText("1000");
}

void MainWindow::on_pushButton_2000_d_clicked()
{
    ui->lineEdit_d->setText("2000");
}

void MainWindow::on_pushButton_5000_d_clicked()
{
    ui->lineEdit_d->setText("5000");
}
void MainWindow::on_pushButton_8000_d_clicked()
{
    ui->lineEdit_d->setText("8000");
}
void MainWindow::on_pushButton_10000_d_clicked()
{
    ui->lineEdit_d->setText("10000");
}
void MainWindow::on_pushButton_exit_d_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "询问", "是否要放弃存款？",
                                  QMessageBox::Yes | QMessageBox::No);
    if(reply==QMessageBox::No){return;}
    else{skipToFunction();}
}

void MainWindow::on_pushButton_clear_d_clicked()
{
    ui->lineEdit_d->clear();
    ui->label_tip_d->clear();
}

void MainWindow::on_pushButton_submit_d_clicked()
{
    QString amnt = ui->lineEdit_d->text();
    if(amnt.right(2)=="00" && (amnt.length()<= 4 || amnt=="10000")){
        QString msg="DEPO "+amnt;
        tcp->write(msg.toUtf8());
        connect(tcp, &QTcpSocket::readyRead, &loop, &QEventLoop::quit);
        loop.exec();
        QByteArray r = tcp->readAll();
        if (r.toStdString()=="525 OK!"){
            QMessageBox::information(this, "Info", "存款成功！");
            skipToFunction();
           }
        else{
            QMessageBox::information(this, "Info", "存款失败！请重新尝试！");
            return;
        }
    }else{
        ui->label_tip_d->setText("输入金额必须为整百数！并且一次存款金额不能超过10000￥！");
        return;
    }
}



