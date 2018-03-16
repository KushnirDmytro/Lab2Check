#include <cmath>
#include <iostream>
#include <iomanip>

#include "integral.h"


using namespace integral;


const double 
    integral :: INITIAL_DELTA = 1,
    integral :: STEP_DELTA_MULTIPLIER = 0.5,
    integral :: EPS = 1e-7;


const int
    integral :: OUTPUT_PRECISION = 6;


double integral :: calc_s(Point &p1, Point &p2){
    return std::abs(p1.x - p2.x) * std::abs(p1.y - p2.y);
}


std::ostream& integral :: operator<<(std::ostream &strm, Result &target){
    std::streamsize initial_precision = strm.precision();

    strm<<std::fixed<<std::setprecision(integral :: OUTPUT_PRECISION);   
    strm<<target.result<<std::endl;
    strm<<target.absolute_error<<" "<<target.relative_error<<std::endl;

    strm<<std::fixed<<std::setprecision(initial_precision);

    return strm;
}