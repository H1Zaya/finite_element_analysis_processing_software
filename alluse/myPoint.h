#ifndef MYPOINT_H
#define MYPOINT_H

#include <QPointF>

class myPoint {
public:
    // 构造函数

    double getX() const;  // 获取横坐标
    double getY() const;  // 获取纵坐标
    int getId() const;    // 获取点的编号

    void setX(double x);  // 设置横坐标
    void setY(double y);  // 设置纵坐标
    void setId(int id);    // 设置点的编号

    QPointF toPointF() const ;

    private:
        double m_x;    // 横坐标
        double m_y;    // 纵坐标
        int m_id;      // 点的编号
};

#endif // MYPOINT_H
