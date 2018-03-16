#include "integral.h"

using namespace integral;

Result :: Result(
    double result,
    double absolute_error,
    double relative_error
)
{
    this->result = result;
    this->absolute_error = absolute_error;
    this->relative_error = relative_error;    
}