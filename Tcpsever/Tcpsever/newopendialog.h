#ifndef NEWOPENDIALOG_H
#define NEWOPENDIALOG_H

#include <QDialog>

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
    void sendusers(QString userid, QString passwd);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::NewopenDialog *ui;

};

#endif // NEWOPENDIALOG_H
