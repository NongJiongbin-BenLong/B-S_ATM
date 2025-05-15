#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);   //界面初始化
    ui->setport->setText("2525");   //默认端口号
    ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(0, 222, 0)");   //监听按钮初始颜色
    m_sever = new QTcpServer(this);   //创建服务器

    connect(m_sever,&QTcpServer::newConnection,this,[=]()
    {   //监听连接
        qDebug()<<"connect";
        tcp = m_sever->nextPendingConnection();
        //连接成功后，显示连接状态
        m_status->setPixmap(QPixmap(":/image/success.jpg").scaled(20,20));
        connect(tcp,&QTcpSocket::readyRead,this,[=]
        {   //接收数据
            QByteArray data = tcp->readAll();
            //将接收到的数据显示在界面上，且写进记录
            op_time = QDateTime::currentDateTime();
            ui->record->append(op_time.toString());
            ui->record->append("ATM: "+data);
            file_record("ATM: "+data);
            //信息处理
            msgHandle(data);
        });
        connect(tcp,&QTcpSocket::disconnected,this,[=]()
        {   //断开连接
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
{   //监听状态启动
    unsigned short port = ui->setport->text().toUShort();
    m_sever->listen(QHostAddress::Any,port);
    ui->listenButton->setDisabled(true);
    ui->listenButton->setStyleSheet("color: rgb(255, 255, 255);background-color: rgb(255, 0, 0)");
    ui->record->append("开启监听......");

}

void MainWindow::listTableShow()
{   //用户信息列表显示
    ui->ListTable->setRowCount(0);
    //数据库连接
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
    //数据查询
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users (userid TEXT PRIMARY KEY, passwd TEXT, money DOUBLE)");
    query.prepare("SELECT * FROM users");
    if(query.exec())
    {
        row = 0;
        while(query.next())
        {   //显示信息
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
{   //记录文件
    QString filepath = QCoreApplication::applicationDirPath() + "/record.txt";
    QString content = QDateTime::currentDateTime().toString()+": "+msg+'\n';
    QFile file(filepath);
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {   //打开文件失败
        qDebug()<<"无法打开文件："<<file.errorString();
        return;
    }
    QTextStream stream(&file);
    stream << content.toUtf8();
    file.close();
}

void MainWindow::on_NewopenButton_clicked()
{   //开户按钮
    newopendialog = NULL;
    //开户对话框
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
{   //销户按钮
    int selectedRow = ui->ListTable->currentRow();
    if (selectedRow < 0)
    {   //选择错误
        QMessageBox::information(this, "选择错误", "请选择要删除的行");
        qDebug()<<"error! 错误选择的行!";
        ui->record->append("error! 错误选择的行!");
        return;
    }
    //获取查询键
    QString Deluserid = ui->ListTable->item(selectedRow, 0)->text();
    //进行数据库删除操作
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
    //删除成功后更新表格并显示记录结果
    listTableShow();
    qDebug()<<"Delete Success! userid="+Deluserid;
    ui->record->append("Delete Success! userid= "+ Deluserid);
    QMessageBox::information(this,"销户提醒","销户成功!");
    QString msg = "Delete Success! userid="+Deluserid;
    file_record("delete userid= "+Deluserid);
}

void MainWindow::msgHandle(QByteArray data)
{   //信息处理
    //信息预处理
    QString str1(data);
    QStringList parts = str1.split(" ");
    parts[0] = parts[0].replace("\r", "");
    parts[0] = parts[0].replace("\n", "");
    qDebug()<<parts[0];

    if(!ask)    //未登录
    {
        //信息处理：HELO，用户查询
        if(parts[0] == "HELO") 
        {
            qDebug()<<"HELO_rcv";
            userid = parts[1].replace("\n","");
            userid = userid.replace("\r", "");
            //数据库连接查询用户名
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
                    QString msg ="500 AUTH REQUIRE\n";
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

        //信息处理：PASS，密码验证
        else if(parts[0] == "PASS")
        {
            qDebug()<<"PASS_rcv";
            passwd = parts[1].replace("\n","");
            passwd = passwd.replace("\r", "");
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
                    {   //密码正确
                        QString msg = "525 OK!\n";
                        file_record(userid+" PASS ACK");
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                        ask = true;
                    }
                    else
                    {   //密码错误
                        QString msg ="401 ERROR!";
                        file_record(userid+" PASS False");
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                    }

                }
                else
                {   //密码验证出错
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

        //信息处理：SIGN，注册
        else if(parts[0] == "SIGN")
        {
            qDebug()<<"SIGN_rcv";
            userid=parts[1].replace("\n","");
            passwd=parts[2].replace("\n","");
            if(userid == nullptr || passwd == nullptr)
            {   //错误处理
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
                            //更新列表记录显示，对客户端进行成功应答
                            listTableShow();
                            qDebug()<<"525 OK!";
                            QString msg ="525 OK!\n";
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

        //信息处理：BYE，结束连接
        else if(parts[0] == "BYE")
        {
            qDebug()<<"BYE_rcv";
            ask = false;
            QString msg ="BYE\n";
            file_record(userid+" "+msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
            //3秒后若客户端未断开连接则断开
            QTimer::singleShot(3000, this, [=](){
                if (tcp->state() == QAbstractSocket::ConnectedState){
                    tcp->close();
                }
            });
        }
        //信息处理：BYE-结束

        else
        {   //错误处理
            qDebug()<<"401 ERROR! 未登录或无效指令!";
            QString msg ="401 ERROR!";
            file_record(msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
        }
    }

    else    //已登录
    {
        //信息处理：BALA，余额查询
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
                    QString msg = "AMNT:" + QString::number(amount) + "\n";
                    file_record(userid+" BALA "+QString::number(amount));
                    tcp->write(msg.toUtf8());
                    ui->record->append("服务器:"+msg);
                    db.close();
                }
                else
                {   //余额查询出错
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

        //信息处理：WDRA，取款
        else if(parts[0] == "WDRA")
        {
            qDebug()<<"WDRA_rcv";
            parts[1] = parts[1].replace("\r", "");
            parts[1] = parts[1].replace("\n", "");
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
                    {   //余额充足
                        amount = amount-amnt;
                        query.prepare("UPDATE users SET money = :money WHERE userid = :userid");
                        query.bindValue(":money",amount);
                        query.bindValue(":userid",userid);
                        if(query.exec())
                        {
                            QString msg = "525 OK!\n";
                            file_record(userid+" WDRA "+QString::number(amnt)+" ;BALA "+QString::number(amount));
                            tcp->write(msg.toUtf8());
                            ui->record->append("服务器:"+msg);
                            db.close();
                            listTableShow();
                        }
                        else
                        {   //取款失败
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
                    {   //余额不足
                        QString msg ="401 ERROR!";
                        file_record(userid+" WDRA False :insufficient balance! BALA "+QString::number(amount));
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                    }

                }
                else
                {   //取款失败
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

        //信息处理：DEPO，存款
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
                {   //存款
                    amount = query.value(0).toDouble();
                    amount = amount + amnt;
                    query.prepare("UPDATE users SET money = ? WHERE userid = ?");
                    query.addBindValue(amount);
                    query.addBindValue(userid);
                    if(query.exec())
                    {   //存款成功
                        QString msg = "525 OK!\n";
                        file_record(userid+" DEPO "+QString::number(amnt)+" ;BALA "+QString::number(amount));
                        tcp->write(msg.toUtf8());
                        ui->record->append("服务器:"+msg);
                        db.close();
                        listTableShow();
                    }
                    else
                    {   //存款失败
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
                {   //存款失败
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

        //信息处理：BYE，断开连接
        else if(parts[0] == "BYE")
        {
            qDebug()<<"BYE_rcv";
            ask = false;
            QString msg ="BYE\n";
            file_record(userid+" "+msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
            //3秒后若客户端未断开连接则断开
            QTimer::singleShot(3000, this, [=](){
                if (tcp->state() == QAbstractSocket::ConnectedState){
                    tcp->close();
                }
            });
        }
        //信息处理：BYE-结束

        else
        {   //错误处理
            qDebug()<<"401 ERROR! 无效的命令!";
            QString msg ="401 ERROR!";
            file_record(msg);
            tcp->write(msg.toUtf8());
            ui->record->append("服务器:"+msg);
        }
    }

}

void MainWindow::recvusers(QString userid, QString passwd)   //接收开户信息
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
                query.bindValue(":money", 100000);
                if(!query.exec())
                {
                    //错误处理
                    QSqlError qerror=query.lastError();
                    qDebug()<<qerror;
                    QMessageBox::critical(this,tr("错误"),tr("列表创建错误!"));
                    return;
                }
                //更新列表记录显示，对客户端进行成功应答
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
