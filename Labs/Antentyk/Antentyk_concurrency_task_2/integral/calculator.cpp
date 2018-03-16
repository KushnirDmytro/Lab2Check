#include <cmath>


#include "integral.h"   


using namespace integral;


Calculator :: Calculator (settings :: SettingsHolder settings){
    this->A = settings.A;
    this->B = settings.B;
    this->C = settings.C;
}


double Calculator :: evaluate(Point &p){
    double result = 0;

    double summond = -1;
    summond *= A;
    summond *= exp((-1) * B * sqrt(0.5 * (p.x * p.x + p.y * p.y)));

    result += summond;

    summond = -1;
    summond *= exp(0.5 * (cos(C * p.x) + cos(C * p.y)));

    result += summond;

    result += A;
    result += exp(1);

    return result;
}