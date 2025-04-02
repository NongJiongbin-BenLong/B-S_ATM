#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "newopendialog.h"
#include <QTableWidgetItem>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QLabel>
#include <QSqlDatabase>
#include <QString>
#include <QTime>
#include <QFile>
#include <QTextStream>

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
    void on_listenButton_clicked();

    void on_NewopenButton_clicked();

    void on_DelButton_clicked();

    void msgHandle(QByteArray data);//信息处理函数

    void listTableShow();//列表显示函数

    void recvusers(QString userid, QString passwd);

    void file_record(QString content);

private:
    Ui::MainWindow *ui;
    QTcpServer *m_sever;
    QTcpSocket *tcp;
    QLabel *m_status;
    QSqlDatabase db;
    QString userid;
    QString passwd;
    double amount;
    bool ask = false;
    QDateTime op_time;
    NewopenDialog *newopendialog;
    QTableWidgetItem *item1;
    QTableWidgetItem *item2;
    int row;

};
#endif // MAINWINDOW_H
