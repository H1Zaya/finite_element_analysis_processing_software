#ifndef MYMATERIAL_H
#define MYMATERIAL_H

#include <QString>

class MyMaterial {
public:

    double getE();
    void setE(double E);

    double getNu();
    void setNu(double nu);

private:
    double m_E;  // 弹性模量
    double m_nu; // 泊松比
};

#endif // MYMATERIAL_H
