#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mydrawwidget.h"


#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void getRectData(MyDrawWidget* widget,QLineEdit* lineEditXY,QLineEdit* lineEditW,QLineEdit* lineEditH);
    void getCircleData(MyDrawWidget* widget,QLineEdit* lineEditXY,QLineEdit* lineEditR);
    void clearPicButton(MyDrawWidget* widget);
    void doMesh(MyDrawWidget* widget);

    void getMaterialData(MyDrawWidget* widget, QLineEdit* lineEditE, QLineEdit* lineEditNiu);

    void updateTextEdit(bool shouldUpdate, QString str);
    void calculation(MyDrawWidget* widget);
    void showdisplacement(MyDrawWidget* widget);
    void showstrain(MyDrawWidget* widget, QLineEdit* lineEdit);
    void showstress(MyDrawWidget* widget, QLineEdit* lineEdit);




private slots:
    void onUpdateTextEdit(bool shouldUpdate, QString str);


private:
    Ui::MainWindow *ui;
    QTextEdit* textEdit;
    static QRegularExpression rx; // 定义一个正则表达式匹配数字+逗号+数字的形式
    static QRegularExpression rx2; // 定义一个正则表达式匹配数字的形式

};
#endif // MAINWINDOW_H
