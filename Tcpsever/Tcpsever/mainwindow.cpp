#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QByteArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->setport->setText("2525");
    ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 222, 0)");
    m_sever = new QTcpServer(this);

    connect(m_sever,&QTcpServer::newConnection,this,[=]()
            {
                qDebug()<<"connect";
                tcp = m_sever->nextPendingConnection();

                m_status->setPixmap(QPixmap(":/image/success.jpg").scaled(20,20));
                connect(tcp,&QTcpSocket::readyRead,this,[=]
                        {
                            QByteArray data = tcp->readAll();
                            op_time = QDateTime::currentDateTime();
                            ui->record->append(op_time.toString());
                            ui->record->append("ATM: "+data);
                            file_record("ATM: "+data);
                            msgHandle(data);
                        });
                connect(tcp,&QTcpSocket::disconnected,this,[=]()
                        {
                            tcp->close();
                            tcp->deleteLater();
                            m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
                            ui->listenButton->setEnabled(true);
                            ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 222, 0)");
                        });
            });

    //状态栏
    m_status = new QLabel(this);
    m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
    ui->statusbar->addWidget(new QLabel("链接状态: "));
    ui->statusbar->addWidget(m_status);
    ui->ListTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listTableShow();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listenButton_clicked()
{
    unsigned short port = ui->setport->text().toUShort();
    m_sever->listen(QHostAddress::Any,port);
    ui->listenButton->setDisabled(true);
    ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(255, 0, 0)");
    ui->record->append("开启监听......");

}

void MainWindow::listTableShow()
{
    ui->ListTable->setRowCount(0);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("my_database.db");
    db.open();
    if(!db.open())
    {
        //错误处理
        qDebug()<<db.lastError();
        QMessageBox::critical(this,tr("错误"),tr("数据库打开失败!"));
        return;
    }
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users (userid TEXT PRIMARY KEY, passwd TEXT, money DOUBLE)");
    query.prepare("SELECT * FROM users");
    if(query.exec())
    {
        row = 0;
        while(query.next())
        {
            item1 = new QTableWidgetItem(query.value(0).toString());
            item2 = new QTableWidgetItem(query.value(2).toString());
            ui->ListTable->insertRow(row);
            ui->ListTable->setItem(row,0,item1);
            ui->ListTable->setItem(row,1,item2);
            ui->ListTable->update();
            row ++;
        }
    }
    db.close();
}

void MainWindow::file_record(QString msg)
{
    QString filepath = QCoreApplication::applicationDirPath() + "/record.txt";
    QString content = QDateTime::currentDateTime().toString()+": "+msg+'\n';
    QFile file(filepath);
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        qDebug()<<"无法打开文件："<<file.errorString();
        return;
    }
    QTextStream stream(&file);
    stream << content.toUtf8();
    file.close();
}

void MainWindow::on_NewopenButton_clicked()
{
    newopendialog = NULL;
    newopendialog = new NewopenDialog(this);
    newopendialog->show();
    if(newopendialog->isMinimized())
    {
        newopendialog->showNormal();
    }
    newopendialog->raise();
    connect(newopendialog,&NewopenDialog::sendusers,this,&MainWindow::recvusers);
}

void MainWindow::on_DelButton_clicked()
{
    int selectedRow = ui->ListTable->currentRow();
    if (selectedRow < 0)
    {
        QMessageBox::information(this, "选择错误", "请选择要删除的行");
        qDebug()<<"error! 错误选择的行!";
        ui->record->append("error! 错误选择的行!");
        return;
    }
    //获取查询键
    QString Deluserid = ui->ListTable->item(selectedRow, 0)->text();
    //进行删除操作
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("my_database.db");
    db.open();
    QSqlQuery query;
    query.prepare("DELETE FROM users WHERE userid = :userid");
    query.bindValue(":userid",Deluserid);
    if (!query.exec())
    {
        QMessageBox::critical(this, "删除错误", "无法删除数据：" + query.lastError().text());
        qDebug()<<"error! 无法删除数据!";
        ui->record->append("error! 无法删除数据!");
        return;
    }
    listTableShow();
    qDebug()<<"Delete Success! userid="+Deluserid;
    ui->record->append("Delete Success! userid= "+ Deluserid);
    QMessageBox::information(this,"销户提醒","销户成功!");
    QString msg = "Delete Success! userid="+Deluserid;
    file_record("delete userid= "+Deluserid);
}

void MainWindow::msgHandle(QByteArray data)
{
    QString str1(data);
    QStringList parts = str1.split(" ");
    qDebug()<<parts[0];

    if(!ask)
    {
        //信息处理：HELO
        if(parts[0] == "HELO")
        {
            qDebug()<<"HELO_rcv";
            userid = parts[1];
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("my_database.db");
            db.open();
            if(!db.open())
            {
                //数据库错误处理
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
                qDebug()<<db.lastError();
                return;
            }
            else
            {
                //查找用户
                QSqlQuery query;
                query.prepare("SELECT * FROM users WHERE userid = ?");
                query.addBindValue(userid);
                if(query.exec() && query.next())
                {
                    //用户存在
                    QString msg ="500 AUTH REQUIRED!";
                    file_record("LOAD "+userid);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
                else
                {
                    //用户不存在
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
            }
        }
        //信息处理：HELO-结束

        //信息处理：PASS
        else if(parts[0] == "PASS")
        {
            qDebug()<<"PASS_rcv";
            passwd = parts[1];
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("my_database.db");
            db.open();
            if(!db.open())
            {
                //数据库错误处理
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
                qDebug()<<db.lastError();
                return;
            }
            else
            {
                //查找用户密码
                QSqlQuery query;
                query.prepare("SELECT passwd FROM users WHERE userid = ?");
                query.addBindValue(userid);
                if(query.exec() && query.next())
                {
                    QString true_passwd = query.value(0).toString();
                    if(passwd == true_passwd)
                    {
                        QString msg = "525 OK!";
                        file_record(userid+" PASS ACK");
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                        ask = true;
                    }
                    else
                    {
                        QString msg ="401 ERROR!";
                        file_record(userid+" PASS False");
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                    }

                }
                else
                {
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
            }
        }
        //信息处理：PASS-结束

        //信息处理：SIGN
        else if(parts[0] == "SIGN")
        {
            qDebug()<<"SIGN_rcv";
            userid=parts[1];
            passwd=parts[2];
            if(userid == nullptr || passwd == nullptr)
            {
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
            }
            else
            {
                //打开数据库
                db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName("my_database.db");
                if(!db.open())
                {
                    //数据库错误处理
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    qDebug()<<db.lastError();
                    return;
                }
                else
                {
                    QSqlQuery query;
                    if(query.exec("CREATE TABLE IF NOT EXISTS users (userid TEXT PRIMARY KEY, passwd TEXT, money DOUBLE)"))
                    {
                        //检查用户是否存在
                        query.prepare("SELECT * FROM users WHERE userid = ?");
                        query.addBindValue(userid);
                        if(query.exec() && query.next())
                        {
                            //错误处理
                            qDebug()<<"401 error! 该用户已存在!";
                            QString msg ="401 ERROR!";
                            file_record(msg);
                            tcp->write(msg.toUtf8());
                            ui->record->append("服务器:"+msg);
                        }
                        else
                        {
                            //插入新用户到users表
                            query.prepare("INSERT INTO users (userid, passwd, money) VALUES (:userid, :passwd, :money)");
                            query.bindValue(":userid", userid);
                            query.bindValue(":passwd", passwd);
                            query.bindValue(":money", 0);
                            if(!query.exec())
                            {
                                //错误处理
                                QSqlError qerror=query.lastError();
                                qDebug()<<qerror;
                                QString msg ="401 ERROR!";
                                file_record(msg);
                                tcp->write(msg.toUtf8());
                                ui->record->append("服务器:"+msg);
                                return;
                            }
                            listTableShow();
                            qDebug()<<"525 OK!";
                            QString msg ="525 OK!";
                            file_record("SIGN "+userid+" "+passwd+" ACK");
                            tcp->write(msg.toUtf8());
                            ui->record->append("服务器:"+msg);
                            db.close();
                        }
                    }
                }
            }
        }
        //信息处理：SIGN-结束

        //信息处理：BYE
        else if(parts[0] == "BYE")
        {
            qDebug()<<"BYE_rcv";
            ask = false;
            QString msg ="BYE";
            file_record(userid+" "+msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
            tcp->close();
            tcp->deleteLater();
            m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
            ui->listenButton->setEnabled(true);
            ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 222, 0)");
        }
        //信息处理：BYE-结束

        else
        {
            qDebug()<<"401 ERROR! 未登录或无效指令!";
            QString msg ="401 ERROR!";
            file_record(msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
        }
    }

    else
    {
        //信息处理：BALA
        if(parts[0] == "BALA")
        {
            qDebug()<<"BALA_rcv";
            qDebug()<<userid;
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("my_database.db");
            db.open();
            if(!db.open())
            {
                //数据库错误处理
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
                qDebug()<<db.lastError();
                return;
            }
            else
            {
                //查找用户余额
                QSqlQuery query;
                query.prepare("SELECT * FROM users WHERE userid = ?");
                query.addBindValue(userid);
                if(query.exec() && query.next())
                {
                    amount = query.value(2).toDouble();
                    QString msg = "AMNT " + QString::number(amount);
                    file_record(userid+" BALA "+QString::number(amount));
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
                else
                {
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
            }
        }
        //信息处理：BALA-结束

        //信息处理：WDRA
        else if(parts[0] == "WDRA")
        {
            qDebug()<<"WDRA_rcv";
            double amnt = parts[1].toDouble();
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("my_database.db");
            db.open();
            if(!db.open())
            {
                //数据库错误处理
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
                qDebug()<<db.lastError();
                return;
            }
            else
            {
                //查找用户余额
                QSqlQuery query;
                query.prepare("SELECT money FROM users WHERE userid = ?");
                query.addBindValue(userid);
                if(query.exec() && query.next())
                {
                    amount = query.value(0).toDouble();
                    if(amount >= amnt)
                    {
                        amount = amount-amnt;
                        query.prepare("UPDATE users SET money = :money WHERE userid = :userid");
                        query.bindValue(":money",amount);
                        query.bindValue(":userid",userid);
                        if(query.exec())
                        {
                            QString msg = "525 OK!";
                            file_record(userid+" WDRA "+QString::number(amnt)+" ;BALA "+QString::number(amount));
                            tcp->write(msg.toUtf8());
                            ui->record->append("服务器:"+msg);
                            db.close();
                            listTableShow();
                        }
                        else
                        {
                            QSqlError qerror=query.lastError();
                            qDebug()<<qerror;
                            QString msg ="401 ERROR!";
                            file_record(msg);
                            tcp->write(msg.toUtf8());
                            ui->record->append("服务器:"+msg);
                            db.close();
                        }
                    }
                    else
                    {
                        QString msg ="401 ERROR!";
                        file_record(userid+" WDRA False :insufficient balance! BALA "+QString::number(amount));
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                    }

                }
                else
                {
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    qDebug()<<"未找到相关记录!";
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
            }
        }
        //信息处理：WDRA-结束

        //信息处理：DEPO
        else if(parts[0] == "DEPO")
        {
            qDebug()<<"DEPO_rcv";
            double amnt = parts[1].toDouble();
            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("my_database.db");
            db.open();
            if(!db.open())
            {
                //数据库错误处理
                QString msg ="401 ERROR!";
                file_record(msg);
                tcp->write(msg.toUtf8());
                ui->record->append("服务器:"+msg);
                qDebug()<<db.lastError();
                return;
            }
            else
            {
                //查找用户余额
                QSqlQuery query;
                query.prepare("SELECT money FROM users WHERE userid = ?");
                query.addBindValue(userid);
                if(query.exec() && query.next())
                {
                    amount = query.value(0).toDouble();
                    amount = amount + amnt;
                    query.prepare("UPDATE users SET money = ? WHERE userid = ?");
                    query.addBindValue(amount);
                    query.addBindValue(userid);
                    if(query.exec())
                    {
                        QString msg = "525 OK!";
                        file_record(userid+" DEPO "+QString::number(amnt)+" ;BALA "+QString::number(amount));
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                        listTableShow();
                    }
                    else
                    {
                        QSqlError qerror=query.lastError();
                        qDebug()<<qerror;
                        QString msg ="401 ERROR!";
                        file_record(msg);
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                    }
                }
                else
                {
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QString msg ="401 ERROR!";
                    file_record(msg);
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
            }
        }
        //信息处理：DEPO-结束

        //信息处理：BYE
        else if(parts[0] == "BYE")
        {
            qDebug()<<"BYE_rcv";
            ask = false;
            QString msg ="BYE";
            file_record(userid+" "+msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
            tcp->close();
            tcp->deleteLater();
            m_status->setPixmap(QPixmap(":/image/failed.jpg").scaled(20,20));
            ui->listenButton->setEnabled(true);
            ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 222, 0)");
        }
        //信息处理：BYE-结束

        else
        {
            qDebug()<<"401 ERROR! 无效的命令!";
            QString msg ="401 ERROR!";
            file_record(msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
        }
    }

}

void MainWindow::recvusers(QString userid, QString passwd)
{
    //打开数据库
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("my_database.db");
    if(!db.open())
    {
        //错误处理
        qDebug()<<db.lastError();
        QMessageBox::critical(this,tr("错误"),tr("数据库打开失败!"));
        return;
    }
    else
    {
        QSqlQuery query;
        if(query.exec("CREATE TABLE IF NOT EXISTS users (userid TEXT PRIMARY KEY, passwd TEXT, money DOUBLE)"))
        {
            //检查用户是否存在
            query.prepare("SELECT * FROM users WHERE userid = ?");
            query.addBindValue(userid);
            if(query.exec() && query.next())
            {
                //错误处理
                qDebug()<<"error! 该用户已存在!";
                QMessageBox::information(this,tr("用户名检查"),tr("该用户已存在!"));
                return;
            }
            else
            {
                //插入新用户到users表
                query.prepare("INSERT INTO users (userid, passwd, money) VALUES (:userid, :passwd, :money)");
                query.bindValue(":userid", userid);
                query.bindValue(":passwd", passwd);
                query.bindValue(":money", 0);
                if(!query.exec())
                {
                    //错误处理
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QMessageBox::critical(this,tr("错误"),tr("列表创建错误!"));
                    return;
                }
                listTableShow();
                qDebug()<<"开户成功!";
                ui->record->append("服务器: SIGN "+userid+" "+passwd);
                file_record("服务器: SIGN "+userid+" "+passwd+" ACK");
                QMessageBox::information(this,tr("开户提醒"),tr("开户成功！"));
                db.close();
            }
        }

    }
}









