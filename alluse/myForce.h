#ifndef MYFORCE_H
#define MYFORCE_H

#include <QPointF>

class MyForce
{
public:
    int id;
    QPointF position;
    double sizex;
    double sizey;

    MyForce();
    MyForce(int forceId, const QPointF& forcePosition, double forceSizex, double forceSizey);

};

#endif // MYFORCE_H
