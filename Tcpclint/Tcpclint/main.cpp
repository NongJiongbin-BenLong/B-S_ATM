#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])    //ATM应用创建
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("ATM");
    w.show();
    return a.exec();
}
