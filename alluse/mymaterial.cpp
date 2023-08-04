#include "mymaterial.h"


double MyMaterial::getE()
{
    return m_E;
}

void MyMaterial::setE(double E)
{
    m_E = E;
}

double MyMaterial::getNu()
{
    return m_nu;
}

void MyMaterial::setNu(double nu)
{
    m_nu = nu;
}
