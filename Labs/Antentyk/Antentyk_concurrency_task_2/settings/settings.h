#include <string>
#include <fstream>


namespace settings{
    extern const int 
        MIN_THREADS_NUM,
        MAX_THREADS_NUM,
        MIN_ITERATIONS_NUM,
        MAX_ITERATIONS_NUM,
        OUTPUT_PRECISION;


    extern const std::string
        MAX_X_TOKEN, MIN_X_TOKEN,
        MAX_Y_TOKEN, MIN_Y_TOKEN,
        STANDARD_TOKEN,
        A_TOKEN, B_TOKEN, C_TOKEN;
    extern const std::string
        ITERATIONS_NUM_TOKEN,
        RELATIVE_ERROR_TOKEN,
        ABSOLUTE_ERROR_TOKEN,
        THREADS_NUM_TOKEN;
    extern const std::string
        IS_CONSECUTIVE_TOKEN;


    extern const char
        DELIMITER,
        SETTINGS_DELIMITER;


    class SettingsHolder{
    public:
        void check();

        int ITERATIONS_NUM = 0;
        int THREADS_NUM = 0;

        double MIN_X = 0, MAX_X = 0;
        double MIN_Y = 0, MAX_Y = 0;
        
        double STANDARD = 0;
        double A = 0, B = 0, C = 0;
        double RELATIVE_ERROR = 0, ABSOLUTE_ERROR = 0;

        bool IS_CONSECUTIVE = false;
    };


    class SettingsRetriever{
    public:
        SettingsRetriever(std::string file_name);
        SettingsHolder retrieve();
    private:
        SettingsHolder target;
        std::ifstream file;
        bool retrieve_setting();

        void fail(std::string message);
    };


    void fail(std::string message);


    std::ostream& operator<<(std::ostream &strm, SettingsHolder &target);
}