#include "mainwindow.h"
#include "mydrawwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QIcon icon("C:/Qt/projects/alluse/icons/delete.png");
    a.setWindowIcon(icon);
    MainWindow w;
    w.show();
    return a.exec();
}
