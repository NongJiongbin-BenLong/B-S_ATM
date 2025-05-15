#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//开户对话框头文件
#include "newopendialog.h"
//界面头文件
#include <QMainWindow>
//Tcp相关头文件
#include <QTcpServer>
#include <QTcpSocket>
//相关组件头文件
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
//相关信息文本处理头文件
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
//数据库头文件
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
//时间头文件
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_listenButton_clicked();     //监听按钮


    void on_NewopenButton_clicked();    //开户按钮

    void on_DelButton_clicked();        //销户按钮


    void msgHandle(QByteArray data);    //信息处理函数

    void listTableShow();   //列表显示函数

    void recvusers(QString userid, QString passwd);   //接收开户信息

    void file_record(QString content);   //文件记录

private:
    Ui::MainWindow *ui;     //界面指针
    QTcpServer *m_sever;    //服务器指针
    QTcpSocket *tcp;        //客户端指针
    QLabel *m_status;       //状态显示标签
    QSqlDatabase db;        //数据库
    QString userid;         //用户名
    QString passwd;         //密码
    double amount;          //余额
    bool ask = false;       //信号确认
    QDateTime op_time;      //开户时间
    NewopenDialog *newopendialog;       //开户对话框指针
    QTableWidgetItem *item1;            //列表项指针
    QTableWidgetItem *item2;            //列表项指针
    int row;                          //行数

};
#endif // MAINWINDOW_H
