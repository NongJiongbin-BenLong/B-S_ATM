#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpSocket>
#include <QHostAddress>

#include <QLabel>
#include <QPushButton>
#include <QInputDialog>

#include <QRegExpValidator>
#include <QByteArray>
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

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
    //wait，主界面
    void on_pushButton_login_clicked();     //login按钮

    void on_pushButton_sign_clicked();      //sign按钮


    //sign，注册界面
    void on_pushButton_exit_s_clicked();    //exit按钮

    void on_pushButton_clear_s_clicked();   //clear按钮

    void on_pushButton_sign_s_clicked();    //sign按钮

    void response_on_pushButton_sign_s_clicked(QStringList parts);    //sign按钮响应

    void on_lineEdit_userid_s_editingFinished();    //userid输入完成

    void response_on_lineEdit_userid_s_editingFinished(QStringList parts);    //userid输入完成响应

    //login，登录界面
    void on_pushButton_exit_l_clicked();    //exit按钮

    void on_pushButton_clear_l_clicked();   //clear按钮

    void on_pushButton_next_l_clicked();    //next按钮

    void response_on_pushButton_next_l_clicked(QStringList parts);    //next按钮响应

    //password，密码验证界面
    void on_pushButton_exit_p_clicked();    //exit按钮

    void on_pushButton_clear_p_clicked();   //clear按钮

    void on_pushButton_login_p_clicked();   //login按钮

    void response_on_pushButton_login_p_clicked(QStringList parts);    //login按钮响应


    //function，功能选择界面
    void on_pushButton_withdrawal_f_clicked();   //withdrawal取款按钮

    void on_pushButton_deposit_f_clicked();      //deposit存款按钮

    void on_pushButton_balance_f_clicked();      //balance余额查询按钮

    void response_on_pushButton_balance_f_clicked(QStringList parts);        //balance余额查询按钮响应

    void on_pushButton_exit_f_clicked();        //exit按钮


    //withdrawal，取款界面
    void on_pushButton_100_wd_clicked();

    void on_pushButton_200_wd_clicked();

    void on_pushButton_500_wd_clicked();

    void on_pushButton_800_wd_clicked();

    void on_pushButton_1000_wd_clicked();

    void on_pushButton_2000_wd_clicked();

    void on_pushButton_3000_wd_clicked();

    void on_pushButton_5000_wd_clicked();

    void on_pushButton_exit_wd_clicked();    //exit按钮

    void on_pushButton_clear_wd_clicked();   //clear按钮

    void on_pushButton_submit_wd_clicked();  //submit按钮

    void response_on_pushButton_submit_wd_clicked(QStringList parts);    //submit按钮响应

    //despoit，存款界面
    void on_pushButton_100_d_clicked();

    void on_pushButton_200_d_clicked();

    void on_pushButton_500_d_clicked();

    void on_pushButton_1000_d_clicked();

    void on_pushButton_2000_d_clicked();

    void on_pushButton_5000_d_clicked();

    void on_pushButton_8000_d_clicked();

    void on_pushButton_10000_d_clicked();

    void on_pushButton_exit_d_clicked();    //exit按钮

    void on_pushButton_clear_d_clicked();   //clear按钮

    void on_pushButton_submit_d_clicked();  //submit按钮

    void response_on_pushButton_submit_d_clicked(QStringList parts);    //submit按钮响应

    //set，设置按钮
    void on_m_set_clicked();    //设置按钮，设置服务端ip地址

private:
    Ui::MainWindow *ui;        //ui

    QTcpSocket *tcp;            //tcp


    QLabel *m_status;       //状态栏
    QPushButton *m_set;   //设置按钮

    QString ip;            //服务端ip地址
    QString S_port;        //服务端端口号字符串
    unsigned short port;   //服务端端口号
    QString ActEven;       //活动事件

    void TCPDisconnect();   //断开tcp连接

    void skipToFunction();  //跳转功能选择界面
};
#endif // MAINWINDOW_H
