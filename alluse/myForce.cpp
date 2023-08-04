#include "myForce.h"
#include <QDebug>

MyForce::MyForce(int forceId, const QPointF& forcePosition, double forceSizex, double forceSizey)
    : id(forceId), position(forcePosition), sizex(forceSizex), sizey(forceSizey)
{
    // 构造函数的其他代码
//    qDebug() << "Created myForce object:";
//    qDebug() << "ID:" << id;
//    qDebug() << "Position:" << position;
//    qDebug() << "Sizex:" << sizex;
//    qDebug() << "sizey:" << sizey;
}
