#include <iostream>
#include <iomanip>


#include "settings.h"


const int 
    settings :: MIN_THREADS_NUM = 1,
    settings :: MAX_THREADS_NUM = 2000,
    settings :: MIN_ITERATIONS_NUM = 1,
    settings :: MAX_ITERATIONS_NUM = 100,
    settings :: OUTPUT_PRECISION = 6;

void settings :: fail(std::string message){
    std::cerr<<message<<std::endl;
    exit(EXIT_FAILURE);
}

std::ostream& settings :: operator<<
(
    std::ostream &strm,
    SettingsHolder &target
){
    
    strm<<std::fixed<<std::setprecision(OUTPUT_PRECISION);

    strm<<MIN_X_TOKEN<<" "<<target.MIN_X<<std::endl;
    strm<<MAX_X_TOKEN<<" "<<target.MAX_X<<std::endl<<std::endl;

    strm<<MIN_Y_TOKEN<<" "<<target.MIN_Y<<std::endl;
    strm<<MAX_Y_TOKEN<<" "<<target.MAX_Y<<std::endl<<std::endl;

    strm<<THREADS_NUM_TOKEN<<" "<<target.THREADS_NUM<<std::endl;
    strm<<IS_CONSECUTIVE_TOKEN<<" "<<target.IS_CONSECUTIVE<<std::endl<<std::endl;

    strm<<RELATIVE_ERROR_TOKEN<<" "<<target.RELATIVE_ERROR<<std::endl;
    strm<<ABSOLUTE_ERROR_TOKEN<<" "<<target.ABSOLUTE_ERROR<<std::endl<<std::endl;

    strm<<ITERATIONS_NUM_TOKEN<<" "<<target.ITERATIONS_NUM<<std::endl<<std::endl;

    strm<<A_TOKEN<<" "<<target.A<<std::endl;
    strm<<B_TOKEN<<" "<<target.B<<std::endl;
    strm<<C_TOKEN<<" "<<target.C<<std::endl<<std::endl;

    strm<<STANDARD_TOKEN<<" "<<target.STANDARD<<std::endl;

    return strm;
}