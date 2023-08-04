#include "calc.h"

using namespace std;

double aijk(double x2, double x3, double y2, double y3)
{
    return (x2 * y3 - x3 * y2);
}

double bijk(double y2, double y3)
{
    return (y2 - y3);
}

double cijk(double x2, double x3)
{
    return (-x2 + x3);
}


QVector<QVector<double>> createElementMatrix(myTriangle sanjiao, QVector<myPoint> tempnodes, MyMaterial material){

    double v=0.3;
    double E = 2e9;
    double t = 1.0;
    if(material.getNu())
        v = material.getNu();
    if(material.getE())
        E = material.getE();

    QVector<QVector<double>>ke;
    ke.resize(6); // 设置行数为6

    for (int i = 0; i < 6; ++i) {
        ke[i].resize(6); // 设置每行的列数为6
        for (int j = 0; j < 6; ++j) {
            ke[i][j] = 0; // 将每个元素设置为0
        }
    }
    double a[3], b[3], c[3];


    double xi=tempnodes[sanjiao.getNode1()].getX();
    double yi=tempnodes[sanjiao.getNode1()].getY();
    double xj=tempnodes[sanjiao.getNode2()].getX();
    double yj=tempnodes[sanjiao.getNode2()].getY();
    double xk=tempnodes[sanjiao.getNode3()].getX();
    double yk=tempnodes[sanjiao.getNode3()].getY();

    double A = (1 / 2.00) * (xi * yj + xj * yk + xk * yi - xi * yk - xj * yi - xk * yj);

    //solve the Intermediate parameters ai,aj...
    a[0] = aijk(xj, xk, yj, yk);
    a[1] = aijk(xk, xi, yk, yi);
    a[2] = aijk(xi, xj, yi, yj);
    b[0] = bijk(yj, yk);
    b[1] = bijk(yk, yi);
    b[2] = bijk(yi, yj);
    c[0] = cijk(xj, xk);
    c[1] = cijk(xk, xi);
    c[2] = cijk(xi, xj);

    double h = E * t / (4 * (1 - v * v) * A);


    // 计算单元刚度矩阵
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            ke[2 * i][2 * j] = h*(b[i] * b[j] + c[i] * c[j] * (1 - v) / 2);
            ke[2 * i][2 * j + 1] = h*(v * b[i] * c[j] + c[i] * b[j] * (1 - v) / 2);
            ke[2 * i + 1][2 * j] = h*(v * c[i] * b[j] + b[i] * c[j] * (1 - v) / 2);
            ke[2 * i + 1][2 * j + 1] = h*(c[i] * c[j] + b[i] * b[j] * (1 - v) / 2);
        }
    }


    return ke;
}



QVector<QVector<double>> createKMatrix(QVector<QVector<QVector<double>>> kkk, int in1, int in2, QVector<myTriangle> tri){ // in1 点的个数

    QVector<QVector<double>>KK;

    KK.resize(2*in1); // 设置行数

    for (int i = 0; i < 2*in1; ++i) {
        KK[i].resize(2*in1); // 设置每行的列数为i*in1
        for (int j = 0; j < 2*in1; ++j) {
            KK[i][j] = 0; // 将每个元素设置为0
        }
    }


    for (int i = 0; i < in2; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                int node[3]={tri[i].getNode1(),tri[i].getNode2(),tri[i].getNode3()};
                KK[node[j] * 2][node[k] * 2] += kkk[i][2*j][2*k];
                KK[node[j] * 2][node[k] * 2 + 1] += kkk[i][2*j][2*k+1];
                KK[node[j] * 2 + 1][node[k] * 2] += kkk[i][2*j+1][2*k];
                KK[node[j] * 2 + 1][node[k] * 2 + 1] += kkk[i][2*j+1][2*k+1];
            }
        }
    }

    return KK;
}

void Gauss2_0(double** a, int n) {
    for (int i = 1; i <= n; ++i) // 枚举列
    {
        int Maxx = i;
        for (int j = i + 1; j <= n; ++j) // 找绝对值最大的
            if (fabs(a[j][i]) > fabs(a[Maxx][i]))
                Maxx = j;
        for (int j = 1; j <= n + 1; ++j)
            swap(a[Maxx][j], a[i][j]);
        if (!a[i][i]) // 没有这个未知数，无解（或无数解）
        {
            qDebug()<<i<<"=0,No Solution!\n";
            return;
        }
        for (int j = 1; j <= n; ++j)
        {
            if (j != i)
            {
                double temp = a[j][i] / a[i][i];
                for (int k = i + 1; k <= n + 1; ++k)
                    a[j][k] -= a[i][k] * temp;
            }
        }
    }
    for (int i = 1; i <= n; ++i) {
        a[i][n + 1] /= a[i][i];
    }

}

