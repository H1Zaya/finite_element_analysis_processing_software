#pragma once
#ifndef CALC_H
#define CALC_H

#include <QVector>
#include <QDebug>
#include "myTriangle.h"
#include "myPoint.h"
#include "mymaterial.h"

double aijk(double x2, double x3, double y2, double y3);
double bijk(double y2, double y3);
double cijk(double x2, double x3);

QVector<QVector<double>> createElementMatrix(myTriangle sanjiao, QVector<myPoint> tempnodes, MyMaterial material);
QVector<QVector<double>> createKMatrix(QVector<QVector<QVector<double>>> kkk, int in1, int in2,QVector<myTriangle> tri);

void Gauss2_0(double** a, int n);


#endif // CALC_H
