#include <cmath>

#include "integral.h"

using namespace integral;

Estimator :: Estimator(settings :: SettingsHolder settings){
    this->STANDARD = settings.STANDARD;
}

double Estimator :: get_relative_error(double result){
    return std::abs(result - STANDARD) / STANDARD;
}

double Estimator :: get_absolute_error(double result){
    return std::abs(result - STANDARD);
}