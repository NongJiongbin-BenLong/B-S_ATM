#ifndef NEWOPENDIALOG_H
#define NEWOPENDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QtDebug>

namespace Ui {
class NewopenDialog;
}

class NewopenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewopenDialog(QWidget *parent = nullptr);
    ~NewopenDialog();

signals:
    void sendusers(QString userid, QString passwd); // 发送信号，传递用户名和密码

private slots:
    void on_buttonBox_accepted();   // 点击“确定”按钮时触发的槽函数

    void on_buttonBox_rejected();   // 点击“取消”按钮时触发的槽函数

private:
    Ui::NewopenDialog *ui;  // 界面指针

};

#endif // NEWOPENDIALOG_H
