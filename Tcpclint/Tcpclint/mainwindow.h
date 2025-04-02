#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QPushButton>
#include <QHostAddress>
#include <QEventLoop>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT;


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //wait
    void on_pushButton_login_clicked();

    void on_pushButton_sign_clicked();

    //sign
    void on_pushButton_exit_s_clicked();

    void on_pushButton_clear_s_clicked();

    void on_pushButton_sign_s_clicked();

    void on_lineEdit_userid_s_editingFinished();

    //login
    void on_pushButton_exit_l_clicked();

    void on_pushButton_clear_l_clicked();

    void on_pushButton_next_l_clicked();

    //password
    void on_pushButton_exit_p_clicked();

    void on_pushButton_clear_p_clicked();

    void on_pushButton_login_p_clicked();

    //function
    void on_pushButton_withdrawal_f_clicked();

    void on_pushButton_deposit_f_clicked();

    void on_pushButton_balance_f_clicked();

    void on_pushButton_exit_f_clicked();


    //withdrawal
    void on_pushButton_100_wd_clicked();

    void on_pushButton_200_wd_clicked();

    void on_pushButton_500_wd_clicked();

    void on_pushButton_800_wd_clicked();

    void on_pushButton_1000_wd_clicked();

    void on_pushButton_2000_wd_clicked();

    void on_pushButton_3000_wd_clicked();

    void on_pushButton_5000_wd_clicked();

    void on_pushButton_exit_wd_clicked();

    void on_pushButton_clear_wd_clicked();

    void on_pushButton_submit_wd_clicked();

    //despoit
    void on_pushButton_100_d_clicked();

    void on_pushButton_200_d_clicked();

    void on_pushButton_500_d_clicked();

    void on_pushButton_1000_d_clicked();

    void on_pushButton_2000_d_clicked();

    void on_pushButton_5000_d_clicked();

    void on_pushButton_8000_d_clicked();

    void on_pushButton_10000_d_clicked();

    void on_pushButton_exit_d_clicked();

    void on_pushButton_clear_d_clicked();

    void on_pushButton_submit_d_clicked();

    //set
    void on_m_set_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket *tcp;

    QLabel *m_status;
    QPushButton *m_set;

    QString ip;
    QString S_port;
    unsigned short port;

    QEventLoop loop;

    void TCPDisconnect();

    void skipToFunction();
};
#endif // MAINWINDOW_H
