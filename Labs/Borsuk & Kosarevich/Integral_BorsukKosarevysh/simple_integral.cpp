#include <iostream>
#include <iomanip>      // std::setprecision
#include <math.h>
#include <chrono>
#include <atomic>
#include <utility>

#include "getconfig.h"

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

double langermann_function(double x, double y, int m, int *c, int *a1, int *a2) {
    double result = 0;
    double val;
    for (int i = 0; i < m; ++i) {
        val = pow(x - a1[i], 2.0) + pow(y - a2[i], 2.0);
        result -= c[i] * exp(-val / M_PI) * cos(val * M_PI);
    }
    return result;
}

double compute_integral(std::pair<double, double> xrange, std::pair<double, double> yrange, int xdelta, int ydelta,
                        int m, int *c, int *a1, int *a2) {
    double integral = 0;
    double x, y;
    double xstep = (xrange.second - xrange.first) / xdelta;
    double ystep = (yrange.second - yrange.first) / ydelta;
    for (int i = 0; i < xdelta; ++i) {
        x = xrange.first + i * xstep;
        for (int j = 0; j < ydelta; ++j) {
            y = yrange.first + j * ystep;
            integral += langermann_function(x, y, m, c, a1, a2) * xstep * ystep;
        }
    }
    return integral;
}

double simple_integral(double alpha, double relalpha,
                       std::pair<double, double> xrange, std::pair<double, double> yrange, int xdelta, int ydelta,
                       int m, int *c, int *a1, int *a2) {
    double prev_integral;
    double integral = 0;
    double ferr;
    int counter = 0;
    do {
        prev_integral = integral;
        integral = compute_integral(xrange, yrange, xdelta, ydelta, m, c, a1, a2);
        xdelta *= 2, ydelta *= 2;
        counter++;
        ferr = fabs(integral - prev_integral);
    } while (ferr > alpha || ferr / integral > relalpha || counter < 2);
    return integral;
}

int main(int argc, char* argv[]) {
    std::string filename;
    if (argc > 1) filename = argv[1];
    else filename = "config.txt";

    try {
        Configs configs(filename);

        auto alpha = configs.get_value<double>("alpha");
        auto relalpha = configs.get_value<double>("relalpha");

        auto xrange = configs.get_pair<double>("xrange");
        auto yrange = configs.get_pair<double>("yrange");

        auto xdelta = configs.get_value<int>("xdelta");
        auto ydelta = configs.get_value<int>("ydelta");
        auto m = configs.get_value<int>("m");

        int c[m], a1[m], a2[m];
        configs.get_array<int>("c", c, m);
        configs.get_array<int>("a1", a1, m);
        configs.get_array<int>("a2", a2, m);

        auto start = get_current_time_fenced();
        double result = simple_integral(alpha, relalpha, xrange, yrange, xdelta, ydelta,
                                        m, c, a1, a2);
        auto end = get_current_time_fenced();
        long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << std::setprecision(8) << result << " " << time << std::endl;
    }
    catch (std::string &e) {
        std::cerr << e <<std::endl;
        return -1;
    }

    return 0;
}