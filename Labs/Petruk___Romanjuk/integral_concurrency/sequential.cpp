#include <iostream>
#include <fstream>
#include <limits>
#include <chrono>
#include <cassert>
#include <atomic>
#include <cmath>
#include <exception>
#include <stdexcept>
#include <string>

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    assert(std::chrono::steady_clock::is_steady &&
                   "Timer should be steady (monotonic).");
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d){
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

using namespace std;

struct configuration_t{
    double rel_err, abs_err;
    double x1, x2, y1, y2;
    size_t initial_steps;
    size_t max_steps;
};

configuration_t read_conf(istream& cf){
    ios::fmtflags flags( cf.flags() ); // Save stream state
    cf.exceptions(std::ifstream::failbit); // Enable exception on fail

    configuration_t res;
    string temp;

    try {
        cf >> res.rel_err;
        getline(cf, temp); // Відкидаємо комент.
        cf >> res.abs_err;
        getline(cf, temp);
        cf >> res.x1;
        getline(cf, temp);
        cf >> res.x2;
        getline(cf, temp);
        cf >> res.y1;
        getline(cf, temp);
        cf >> res.y2;
        getline(cf, temp);
        cf >> res.initial_steps;
        getline(cf, temp);
        cf >> res.max_steps;
        getline(cf, temp);
    }catch(std::ios_base::failure &fail) // Life without RAII is hard...
    {
        cf.flags( flags );
        throw; // re-throw exception
    }
    cf.flags( flags );
    if( res.x2 <= res.x1 ) {
        throw std::runtime_error("x1 should be <= x2");
    }
    if( res.y2 <= res.y1 ) {
        throw std::runtime_error("y1 should be <= y2");
    }
    if( res.initial_steps < 10 ) {
        throw std::runtime_error("Too few initial_steps");
    }
    if( res.max_steps < 10 ) {
        throw std::runtime_error("Too small max_steps");
    }
    if( res.abs_err <=0 || res.rel_err <= 0 ) {
        throw std::runtime_error("Errors should be positive");
    }

    return res;
}

double func_to_integrate(const double& x1, const double& x2) {
    //return x*x;
    double sigmas = 0;
    for(int i=-2;i<=2;++i){
        for(int j=-2;j<=2;++j){
        sigmas += 1.0 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6.0) + pow((x2 - 16 * i), 6.0));
        }
    }
    return 1.0 / (0.002 + sigmas);

}

template<typename func_T>
double integrate_1d(func_T func, double x1, const double& x2, const double& y, size_t steps){
    double res = 0;
    double delta_x = (x2 - x1)/steps;
    while(x1<x2)
    {
        res += func(x1, y) * delta_x;
        x1 += delta_x;
    }
    return res;
}

double integrate_2d(double x1, const double& x2, double y1, const double& y2, size_t steps){
    double res = 0;
    double delta_y = (y2 - y1)/steps;
    while(y1<y2){
        //res += func(x1) * delta_x;
        res += integrate_1d(func_to_integrate, x1, x2, y1, steps) * delta_y;
y1 += delta_y;
    }
    return res;
}


//! Якщо передано ім'я файлу -- він використовується. Якщо ні -- шукається файл
//! з іменем seqconf.txt.
int main(int argc, char* argv[]){
    string filename("seqconf.txt");
    if(argc == 2)
        filename = argv[1];
    if(argc > 2) {
        cerr << "Too many arguments. Usage: \n"
                "<program>\n"
                "or\n"
                "<program> <config-filename>\n" << endl;
        return 1;
    }
    ifstream config_stream(filename);
    if(!config_stream.is_open()) {
        cerr << "Failed to open configuration file " << filename << endl;
        return 2;
    }

    configuration_t config;
    try{
        config = read_conf(config_stream);
    }catch (std::exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        return 3;
    }

    auto before = get_current_time_fenced();

    size_t steps = config.initial_steps;
    double cur_res = integrate_2d(config.x1, config.x2, config.y1, config.y2, steps);
    double prev_res = cur_res;
    bool to_continue = true;
    double abs_err = -1; // Just guard value
    double rel_err = -1; // Just guard value
//#define PRINT_INTERMEDIATE_STEPS
    while( to_continue ) {
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << cur_res << " : " << steps << " steps" << endl;
#endif
        prev_res = cur_res;
        steps *= 2;
        cur_res = integrate_2d(config.x1, config.x2, config.y1, config.y2, steps);
        abs_err = fabs(cur_res-prev_res);
        rel_err = fabs( (cur_res-prev_res)/cur_res );
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << '\t' << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
#endif

        to_continue = ( abs_err > config.abs_err );
        to_continue = to_continue && ( rel_err > config.rel_err );
        to_continue = to_continue && (steps < config.max_steps);
    }

    auto time_to_calculate = get_current_time_fenced() - before;

    cout << "Result: " << cur_res << endl;
    cout << "Abs err : rel err " << abs_err << " : " << rel_err << endl;
    cout << "Time: " << to_us(time_to_calculate)  << endl;

    return 0;
}
