#include "myPoint.h"


double myPoint::getX() const{
    return m_x;
}

double myPoint::getY() const{
    return m_y;
}

int myPoint::getId() const{
    return m_id;
}

void myPoint::setX(double x) {
    m_x = x;
}

void myPoint::setY(double y) {
    m_y = y;
}

void myPoint::setId(int id) {
    m_id = id;
}

QPointF myPoint::toPointF() const {
    return QPointF(m_x, m_y);
}
