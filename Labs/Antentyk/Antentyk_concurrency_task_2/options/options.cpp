#include <iostream>
#include <string>
#include <string.h>

#include "options.h"

using namespace options;

void options :: display_help(){
    static std::string help_str = 
"Usage: main [OPTION] [CONFIG_FILE]\n\n\
Integrates Ackley's function according to settings provided in config file\n\n\
-h --help           displays this message and exit\n\
-c --calculate      integrates and outputs the results\n\n\
Output format:\n\
    result_of_integration\n\
    absolute_error relative_error\n\
    time_spent (int microseconds)\n\n\
Config file:\n\
    PARAMETER1=VALUE1;\n\
    PARAMETER2=VALUE2;\n\
    ...\n\
    PARAMETERN=VALUEN;\n\
see examples/config.txt\n\n\
If parameter is not present, it is set to default value (see below)\n\
Parameters list:\n\
    MAX_X: double, default: 0 - maximal value of x to integrate\n\
    MIN_X: double, default: 0 - minimal value of x to integrate\n\
    MAX_Y: double, default: 0 - maximal value of y to integrate\n\
    MIN_Y: double, default: 0 - minimal value of y to integrate\n\
    A: double, default: 0 - a parameter in Ackley's function\n\
    B: double, default: 0 - b parameter in Ackley's function\n\
    C: double, default: 0 - c parameter in Ackley's function\n\
    STANDARD: double, default: 0 - benchmark\n\
    ITERATIONS_NUM: int, default: 0, maximal numbers of iteration to perform\n\
    RELATIVE_ERROR: double, default: 0, maximal relative error that you want to obtain\n\
    ABSOLUTE_ERROR: double, default: 0, maximal absolute error that you want to obtain\n\
    IS_CONSECUTIVE: bool(0 or 1), default: 0, indicates if consecutive algorithm should perform\n\
    THREADS_NUM: int, default: 0, indicates how many threads to launch";

    std::cout<<help_str<<std::endl;
}

std::string options :: retrieve(int argc, char ** argv){
    static std::string hint = "Run with --help option to learn about the usage";

    if(argc == 1){
        std::cerr<<"No arguments provided"<<std::endl;
        std::cerr<<hint<<std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc > 3){
        std::cerr<<"Too many arguments provided"<<std::endl;
        std::cerr<<hint<<std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc == 2){
        ++argv;
        if(
            strcmp(options :: HELP_SHORT_TOKEN.c_str(), *argv) == 0 ||
            strcmp(options :: HELP_LONG_TOKEN.c_str(), *argv) == 0
        ){
            display_help();
            exit(EXIT_SUCCESS);
        }

        std::cerr<<"Invalid argument"<<std::endl;
        std::cerr<<hint<<std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc == 3){
        ++argv;
        if(
            strcmp(options :: CALC_SHORT_TOKEN.c_str(), *argv) != 0 &&
            strcmp(options :: CALC_LONG_TOKEN.c_str(), *argv) != 0
        ){
            std::cerr<<"-c key expected"<<std::endl;
            std::cerr<<hint<<std::endl;
            exit(EXIT_FAILURE);
        }

        ++argv;
        std::string result = *argv;

        return result;
    }
}