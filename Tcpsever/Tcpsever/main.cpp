#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])    //应用创建
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
