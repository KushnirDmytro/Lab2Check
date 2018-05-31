#include <iostream>
#include <chrono>
#include <atomic>
#include <assert.h>
#include <fstream>
#include <string>
#include <math.h>
#include <sstream>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::map;
using std::vector;
using std::thread;
using std::mutex;

inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

struct Parameters {
    static const int m = 5;
    double a1[m] = {1.0, 2.0, 1.0, 1.0, 5.0};
    double a2[m] = {4.0, 5.0, 1.0, 2.0, 4.0};
    double c[m] = {2.0, 1.0, 4.0, 7.0, 2.0};
    double x_min = -10,x_max = 10, y_min = -10, y_max = 10;
    double erelative = 0.001, eabsolute = 0.05;
    int steps = 100, max_steps = 1000;
    int n_threads = 4;
    double expect_res = -1.604646665;
};

void integral(const int from, const int to, const Parameters &params, double &result, mutex &m) {
    double res = 0;
    double x1 = params.x_min, y1= params.y_min;
    double dx = (params.x_max - params.x_min) / params.steps;
    double dy = (params.y_max - params.y_min) / params.steps;

    int i = from;
    while (i < to){
        int j = 0;
        while (j < params.steps){
            double r = 0;
            int k = 0;
            while (k < params.m){
                r += params.c[k] * exp(-(1/M_PI) * (pow((((x1 + i * dx) + (x1 + (i+1) * dx))/2 - params.a1[k]), 2) +
                                                    pow((((y1 + j * dy) + (y1 + (j+1) * dy))/2 - params.a2[k]), 2))) *
                     cos(M_PI * (pow((((x1 + i * dx) + (x1 + (i+1) * dx))/2 - params.a1[k]), 2) +
                                 + pow((((y1 + j * dy) + (y1 + (j+1) * dy))/2 - params.a2[k]), 2)));
                k++;
            }
            res += -r * dx * dy;
            j++;
        }
        i++;
    }

    m.lock();
    result += res;
    m.unlock();
}

int main(int argc, char * argv[]) {

    Parameters parameters;
    string filename("configurations.txt");

    std::ifstream config_stream(filename);

    if(!config_stream.is_open()) {
        cerr << "Cannot open configuration file " << filename << endl;
        exit(2);
    }

    map<string, string> parsed_params;
    string line;

    while (config_stream >> line) {
        std::istringstream is_line(line);
        std::string k;
        if (std::getline(is_line, k, '=')) {
            std::string s;
            if (std::getline(is_line, s)) {
                parsed_params[k] = s;
                parsed_params[k];
            }
        }
    }


    config_stream.close();
    auto conf = parsed_params;

    try {
        parameters.n_threads = std::stod(conf.at("threads"));
        parameters.x_min = std::stod(conf.at("x_min"));
        parameters.x_max = std::stod(conf.at("x_max"));
        parameters.y_min = std::stod(conf.at("y_min"));
        parameters.y_max = std::stod(conf.at("y_max"));
        parameters.erelative = std::stod(conf.at("erelative"));
        parameters.eabsolute = std::stod(conf.at("eabsolute"));
        parameters.steps = std::stod(conf.at("steps"));
        parameters.max_steps = std::stoi(conf.at("max_steps"));
    } catch (...) {
        cerr << "Error" << endl;
        exit(3);
    }

    double expect_res = parameters.expect_res;
    double result = 1000;
    double absolute, relative;


    while ((absolute < parameters.eabsolute) && (relative < parameters.erelative)) {

        std::vector<thread> threads;
        std::mutex mtx;

        expect_res = result;
        auto start_time = get_current_time_fenced();

        int step = parameters.steps / parameters.n_threads;

        int x = 0, y = x + step, i = 0;
        double thread_result = 0;
        while (i < parameters.n_threads){
            threads.emplace_back([x, y, &parameters, &thread_result, &mtx] {
                integral(x, y, parameters, thread_result, mtx);
            });
            x = y; y += step;
            i++;
        }

        for (auto& t : threads) {
            t.join();
        }

        result = thread_result;

        auto end_time = get_current_time_fenced();
        std::cout << "abs_err : " << absolute << endl;
        relative = abs(absolute / expect_res);
        std::cout << result << endl;
        absolute = abs(expect_res - result);
        std::cout << result << endl;
        auto total = end_time - start_time;

        std::cout << result << endl;
        std::cout << "Total time: " << to_us(total) << endl;
        std::cout << "Absolute error: " << absolute << endl;
        std::cout << "Relative error: " << relative << endl;
    };

    return 0;
}
