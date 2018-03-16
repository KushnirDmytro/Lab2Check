#include <string>

namespace options{
    extern const std::string
        HELP_LONG_TOKEN,
        HELP_SHORT_TOKEN,
        CALC_LONG_TOKEN,
        CALC_SHORT_TOKEN;
    
    void display_help();

    std::string retrieve(int argc, char ** argv);

    void fail(std::string message);
}