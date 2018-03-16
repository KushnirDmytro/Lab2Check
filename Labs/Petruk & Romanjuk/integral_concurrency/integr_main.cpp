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
#include <thread>
#include <mutex>
#include <vector>

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
    int number_thread;
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
        cf >> res.number_thread;
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
    if( res.number_thread < 1 ) {
        throw std::runtime_error("number of threads should be > 0");
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
    if( res.abs_err <= 0 || res.rel_err <= 0 ) {
        throw std::runtime_error("Errors should be positive");
    }

    return res;
}

void get_parts(int start, int finish, int threads, double *parts) {
    int step = (finish - start) / threads;
    int x_min, x_max;
    for (int i = 0; i < threads; i++) {
        x_min = start + i * step;
        x_max = start + (i + 1) * step;
        parts[i] = x_min;
        parts[i+1] = x_max;
    }
}

double func_to_integrate(const double& x1, const double& x2) {
    double sigmas = 0;
    for(int i =- 2; i <= 2; ++i){
        for(int j =- 2; j <= 2; ++j){
            sigmas += 1.0 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6.0) + pow((x2 - 16 * i), 6.0));
        }
    }
    return 1.0 / (0.002 + sigmas);
}

template<typename func_T>
double integrate_1d(func_T func, double x1, const double& x2, const double& y, size_t steps){
    double res = 0;
    double delta_x = (x2 - x1) / steps;
    while(x1 < x2) {
        res += func(x1, y) * delta_x;
        x1 += delta_x;
    }
    return res;
}

void integrate_2d(double x1, const double& x2, double y1, const double& y2, size_t steps, mutex &mtx, double &result){
    double res = 0;
    double delta_y = (y2 - y1) / steps;
    while(y1 < y2){
        res += integrate_1d(func_to_integrate, x1, x2, y1, steps) * delta_y;
				y1 += delta_y;
    }
    mtx.lock();
    result += res;
    mtx.unlock();
}


//! Якщо передано ім'я файлу -- він використовується. Якщо ні -- шукається файл
//! з іменем conf.txt.
int main(int argc, char* argv[]){
    // Реалізація читання аргументів свідомо примітивна! Щоб і вам якусь роботу
    // залишити. Однак, рекомендую вам таки спробувати написати більш
    // просунуту реалізацію, яка дозволяє писати конфігураційний файл у вигляді:
    // rel_err = 0.001
    // abs_err = 0.05
    // max_iterations = 1000000
    // і т.д.
    string filename("conf.txt");
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

    double parts[config.number_thread + 1] = {0};
    get_parts(config.y1, config.y2, config.number_thread, parts);
    parts[config.number_thread] = config.y2;


    auto before = get_current_time_fenced();

    size_t steps = config.initial_steps;

    vector<thread> myThreads;
    double cur_res = 0;

    mutex mtx;

    for (int i = 0; i < config.number_thread; i++) {
        myThreads.emplace_back(integrate_2d, config.x1, config.x2, parts[i], parts[i+1], steps / config.number_thread, ref(mtx), ref(cur_res));
    }

    for (int i = 0; i < myThreads.size(); i++) {
        myThreads[i].join();
    }

    //double cur_res = integrate_2d(config.x1, config.x2, config.y1, config.y2, steps);
    double prev_res = cur_res;
    bool to_continue = true;
    double abs_err = -1; // Just guard value
    double rel_err = -1; // Just guard value
//#define PRINT_INTERMEDIATE_STEPS

    while( to_continue ) {
#ifdef PRINT_INTERMEDIATE_STEPS
        cout << cur_res << " : " << steps << " steps" << endl;
#endif
        vector<thread> myThreads;
        prev_res = cur_res;
        steps *= 2;

        cur_res = 0;

        for (int i = 0; i < config.number_thread; i++) {
            myThreads.emplace_back(integrate_2d, config.x1, config.x2, parts[i], parts[i+1], steps / config.number_thread, ref(mtx), ref(cur_res));
        }

        for (int i = 0; i < myThreads.size(); i++) {
            myThreads[i].join();
        }

        //cur_res = integrate_2d(config.x1, config.x2, config.y1, config.y2, steps); // steps / number_thread
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
