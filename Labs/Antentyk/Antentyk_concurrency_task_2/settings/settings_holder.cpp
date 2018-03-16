#include "settings.h"

using namespace settings;

void SettingsHolder :: check(){
    if(
    !IS_CONSECUTIVE &&
    (THREADS_NUM < MIN_THREADS_NUM || THREADS_NUM > MAX_THREADS_NUM)
    )
        settings :: fail("Wrong bound on threads num");

    if(
    IS_CONSECUTIVE && 
    THREADS_NUM != 0
    )
        settings :: fail("Consecutive and non-consecutive simultaneously");

    if(
    ITERATIONS_NUM < MIN_ITERATIONS_NUM ||
    ITERATIONS_NUM > MAX_ITERATIONS_NUM)
        settings :: fail("Wrong bound on iterations num");

    if(MIN_X > MAX_X)
        settings :: fail("MIN_X cannot be greater than MAX_X");
    if(MIN_Y > MAX_Y)
        settings :: fail("MIN_Y cannot be greater than MAX_Y");
    
    
}