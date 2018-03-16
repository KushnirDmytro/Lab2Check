#include <iostream>
#include <iomanip>
#include <math.h>
#include <thread>
#include <atomic>
#include <sstream>
#include "INIReader.h"

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

double task_func(double x, double y, int m, int *c, int *a1,
                 int *a2) { // Easy to change task function. But also is needed to change thread logic.
    double hlp;
    double res = 0;
    for (int i = 0; i < m; ++i) {
        hlp = pow(y - a2[i], 2.0) + pow(x - a1[i], 2.0); // We don't need to calculate pow's 4 times, only twice.
        res += -c[i] * cos(hlp * M_PI) * exp(-hlp / M_PI);
    }
    return res;
}

void integral(double &integral,

              std::pair<double, double> x_r, // _r - range
              std::pair<double, double> y_r,

              int x_d, // _d - delta
              int y_d,

              int m, int *c, int *a1, int *a2) {

    double local_integral = 0;

    double x, y;

    double x_s = (x_r.second - x_r.first) / x_d;
    double y_s = (y_r.second - y_r.first) / y_d;

    for (int i = 0; i < x_d; ++i) {

        x = i * x_s + x_r.first;

        for (int j = 0; j < y_d; ++j) {
            y = y_r.first + j * y_s;
            local_integral += task_func(x, y, m, c, a1, a2) * x_s * y_s;
        }

    }
    integral = local_integral;
}

double thread_integral(int n,
                       double alpha,
                       double relalpha,
                       std::pair<double, double> x_r,
                       std::pair<double, double> y_r,
                       int x_d,
                       int y_d,
                       int m,
                       int *c,
                       int *a1,
                       int *a2
) {

    std::thread threads[n];
    double local_integral[n];

    std::pair<double, double> thread_y_r(y_r.first, y_r.second);

    double curr_integral_val = 0;
    double prev_integral_val = 0;


    int thread_x_d;
    int thread_y_d;

    int k = 0;

    double step_err;

    do {
        k++;

        prev_integral_val = curr_integral_val;
        curr_integral_val = 0;

        for (int i = 0; i < n; ++i) {
            bool hpr = i == n - 1;
            std::pair<double, double> local_x_r(
                    x_r.first + i * (x_r.second - x_r.first) / n, // Method: Divide by threads by x coord.
                    hpr ? x_r.second : x_r.first - (i + 1) * (x_r.first - x_r.second) / n);

            thread_x_d = hpr ? (x_d - i * x_d / n) : (x_d / n);
            thread_y_d = y_d;

            threads[i] = std::thread(integral,
                                     std::ref(local_integral[i]),

                                     local_x_r,
                                     thread_y_r,

                                     thread_x_d,
                                     thread_y_d,
                                     m, c, a1, a2);
        }

        for (int i = 0; i < n; ++i) {

            threads[i].join();
            curr_integral_val += local_integral[i];

        }


        step_err = fabs(curr_integral_val - prev_integral_val);

        x_d *= 2;
        y_d *= 2;

    } while (step_err > alpha || step_err / curr_integral_val > relalpha || k < 2);

    return curr_integral_val;
}

void read_int_arr(INIReader reader, const std::string &section, const std::string &param, int *arr, int m) {
    std::string result = reader.Get(section, param, "");
    std::stringstream stream(result);
    int i = 0;
    while (stream >> arr[i]) { i++; }
    if (i != m) { throw "Len of arr in not equal to m " + param; }
}

int main(int argc, char *argv[]) {

    std::string filename;
    int n_threads;

    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "config.txt";
    }

    if (argc > 2) {
        n_threads = std::stoi(argv[2]);
    } else {
        n_threads = 2;
    }

    try {
        INIReader reader(filename);

        if (reader.ParseError() < 0) {
            std::cout << "Can't load .ini file\n";
            return 1;
        }

        auto alpha = reader.GetReal("config", "alpha", 0.0001);
        auto relalpha = reader.GetReal("config", "relalpha", 0.0001);

        auto x_r = std::make_pair(reader.GetReal("config", "xmin", -10), reader.GetReal("user", "xmax", 10));
        auto y_r = std::make_pair(reader.GetReal("config", "ymin", -10), reader.GetReal("user", "ymax", 10));

        auto x_d = reader.GetInteger("config", "xdelta", -1);
        auto y_d = reader.GetInteger("config", "ydelta", -1);
        auto m = reader.GetInteger("functionconfig", "m", -1);

        int c[m], a1[m], a2[m];
        read_int_arr(reader, "functionconfig", "c", c, m);
        read_int_arr(reader, "functionconfig", "a1", a1, m);
        read_int_arr(reader, "functionconfig", "a2", a2, m);
        auto start = get_current_time_fenced();
        double result = thread_integral(n_threads, alpha, relalpha, x_r, y_r, x_d, y_d,
                                        m, c, a1, a2);
        auto end = get_current_time_fenced();
        long long time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        std::cout << std::setprecision(8) << result << " " << time << std::endl;

        return 0;
    }
    catch (std::string &e) {
        std::cerr << e << std::endl;
        return -1;
    }
}