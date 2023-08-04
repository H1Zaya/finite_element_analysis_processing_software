#ifndef MYDRAWWIDGET_H
#define MYDRAWWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QPixmap>
#include <gmsh.h>
#include <QFile>
#include <cmath>
#include <array>
#include <QMessageBox>
#include <QLineEdit>
#include "myPoint.h"
#include "myTriangle.h"
#include "mymaterial.h"
#include "myForce.h"


using namespace std;

class MyDrawWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyDrawWidget(QWidget *parent = nullptr);
    ~MyDrawWidget();
    void drawRect(QRectF& rect);
    void drawCircle(QPointF& center_draw,double r_draw);
    void clearRectAndCircleData(); // 清空所有数据
    void clearPixmap(); // 清空所有图像
    bool isCircleInRect(QPointF& point1, double r);// 判断圆形是否在矩形内
    void meshPaint(); // 网格绘制
    void storeMaterial(MyMaterial& materialss); // 存储材料参数

    // 添加约束部分函数
    bool isVertexClicked(QPointF& posp); //判断是否点击矩形顶点
    bool isBoundaryClicked(QPointF& posp); // 判断是否点击矩形边界
    void vertexClickReaction(); // 点击矩形顶点后的事件
    void boundaryClickReaction(QPointF& posp); // 右键点击矩形边界后事件

    void drawConstriant(const QPointF point, bool s1, bool s2); // 画出约束标志
    void drawConcentratedForce(const QPointF point, bool s1, bool s2); // 集中约束力绘制
    // 下面这个函数绘制分布力图像, 并且得到均布力的数据, 并且分配给节点等等
    void drawDistributedForce(const QPointF point1, const QPointF point2, int t); // 分布约束力绘制

    QPair<double, double> getLineEditValues(QLineEdit *lineEdit1, QLineEdit *lineEdit2);
    double getLineEditValues2(QLineEdit *lineEdit);


    // 计算函数
    void calcMatrix();

    // 画云图
    void drawDisplacement();
    void drawstrain(int k);
    void drawStress(int k);


    void drawAnswer();

    // 获取颜色
    QColor getColorForValue(double value, double minValue, double maxValue);

    void drawAnswer(double minValue, double maxValue);

signals:
    void updateTextEdit(bool shouldUpdate, QString str);  // 定义更新textedit的信号, 为0的时候才会执行
    void vertexClicked(int index);



protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;



private:
    QRectF m_rect;
    QPointF m_center;
    double m_radius;
    QPointF m_center2;
    double m_radius2;


    array<QPixmap, 100> pixmapArray; // 一个数组用来存储所有pixmap
//    array<int,100> pixmapsign;

    QVector<myPoint> myNodes;
    QVector<myTriangle> myElem;

    QVector<myPoint> nodes2;

    QVector<MyForce> forceInfo; // 储存全部顶点
    QVector<MyForce> vertexForce; // 储存四个顶点


    int sig; // 判断顶点是哪个的标志
    int sig1; // 力对点
    int sig2; // 判断点击了哪条边

    int pixmapnum; // 边界约束集中力作图

    MyMaterial m_material;

    QPointF previousPos;// 添加分布力用


    QVector<int> addCons;
    QVector<double> addForce;

    QVector<double> delta; // 存储各节点位移

    QVector<QVector<double>> strain; // 存储单元应变

    QVector<QVector<double>> stress; // 存储单元应力

    QVector<QVector<double>> ALLMatrix;

    array<double,6> min;
    array<double,6> max;


    bool beforecalc;

private slots:


};

#endif // MYDRAWWIDGET_H
