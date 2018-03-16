#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

inline double de_jong(double x, double y) {
    double sum = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x - 16 * j), 6) + pow((y - 16 * i), 6));
        }
    }
    return 1 / (0.002 + sum);
}

void logic(const std::function<double(double, double)> &func_to_integrate, double &sum, double x_min, double x_max,
           double y_min, double y_max, double N, double M, std::mutex &m) {
    double temp_sum = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            double x1 = (x_min + i * (x_max - x_min) / M),
                    x2 = (x_min + (i + 1) * (x_max - x_min) / M),
                    x = (x1 + x2) / 2,
                    y1 = (y_min + j * (y_max - y_min) / N),
                    y2 = (y_min + (j + 1) * (y_max - y_min) / N),
                    y = (y1 + y2) / 2,
                    dx = x2 - x1,
                    dy = y2 - y1;
            temp_sum += func_to_integrate(x, y) * dx * dy;
        }
    }
    m.lock();
    sum += temp_sum;
    m.unlock();
}

double integrate_1d(std::function<double(double)> const &func, double lower, double upper, int n) {
    double increment = 1.0 / n;

    double integral = 0.0;
    for (double x = lower; x < upper; x += increment) {
        integral += func(x) * increment;
    }
    return integral;
}

void integrate_2d(std::function<double(double, double)> const &func, double &sum, double lower1, double upper1,
                  double lower2, double upper2, int n, std::mutex &m) {
    double increment = 1.0 / n;
    double integral = 0.0;
    for (double x = lower2; x < upper2; x += increment) {
        auto func_x = [=](double y) { return func(x, y); };
        integral += integrate_1d(func_x, lower1, upper1, n) * increment;
    }
    m.lock();
    sum += integral;
    m.unlock();
}

void thread_pool(int q, double &sum, double min_x, double max_x, double min_y, double max_y, double N, double M,
                 std::mutex &m) {
    std::vector<std::thread> threads;
    double t_min_x = min_x,
            step = (max_x - min_x) / q,
            t_max_x = t_min_x + step;
    for (int i = 0; i < q; ++i) {
        if (i == q - 1) {
//            threads.emplace_back(std::thread(integrate_2d, de_jong, std::ref(sum), t_min_x, max_x, min_y, max_y, N, std::ref(m)));   //uses integrate_2d
            threads.emplace_back(
                    std::thread(logic, de_jong, std::ref(sum), t_min_x, max_x, min_y, max_y, N, M,
                                std::ref(m)));                      //uses logic
        } else {
//            threads.emplace_back(std::thread(integrate_2d, de_jong, std::ref(sum), t_min_x, t_max_x, min_y, max_y, N, std::ref(m))); //uses integrate_2d
            threads.emplace_back(
                    std::thread(logic, de_jong, std::ref(sum), t_min_x, t_max_x, min_y, max_y, N, M,
                                std::ref(m)));                    //uses logic
        }
        if (t_max_x + step > max_x) break;
        else {
            t_min_x = t_max_x;
            t_max_x += step;
        }
    }
    for (int i = 0; i < q; ++i) {
        threads[i].join();
    }
}

struct configuration_t {
    int thread_number;
    double rel_err, abs_err;
    double x1, x2, y1, y2;
    int initial_steps, max_steps;
};

using namespace std;

configuration_t read_conf(istream &cf) {
    ios::fmtflags flags(cf.flags());
    cf.exceptions(std::ifstream::failbit);

    configuration_t res;
    string temp;

    try {
        cf >> res.thread_number;
        getline(cf, temp);
        cf >> res.rel_err;
        getline(cf, temp);
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
    } catch (std::ios_base::failure &fail)
    {
        cf.flags(flags);
        throw;
    }
    cf.flags(flags);
    if (res.thread_number < 0) {
        throw std::runtime_error("thread number should be > 0");
    }
    if (res.x2 <= res.x1) {
        throw std::runtime_error("x1 should be <= x2");
    }
    if (res.y2 <= res.y1) {
        throw std::runtime_error("y1 should be <= y2");
    }
    if (res.initial_steps < 10) {
        throw std::runtime_error("Too few initial_steps");
    }
    if (res.max_steps < 10) {
        throw std::runtime_error("Too small max_steps");
    }
    if (res.abs_err <= 0 || res.rel_err <= 0) {
        throw std::runtime_error("Errors should be positive");
    }

    return res;
}

int main() {
    string filename("config.txt");
    ifstream config_stream(filename);
    if (!config_stream.is_open()) {
        cerr << "Failed to open configuration file " << filename << endl;
        return 2;
    }

    configuration_t config;
    try {
        config = read_conf(config_stream);
    } catch (std::exception &ex) {
        cerr << "Error: " << ex.what() << endl;
        return 3;
    }
    std::mutex mux;
    double sum = 0;
    int q = config.thread_number;
    double x1 = config.x1, x2 = config.x2, y1 = config.y1, y2 = config.y2;
    int init_division = config.initial_steps;
    int max_division = config.max_steps;
    double abs_err = config.abs_err, rel_err = config.rel_err;
    double t_sum = 0, t_abs_err = 0, t_rel_err = 0;
    auto stage1_start_time = get_current_time_fenced();
    for (int i = 0; i < 5; ++i) {
        thread_pool(q, sum, x1, x2, y1, y2, init_division * pow(2, i), init_division * pow(2, i), mux);
        t_abs_err = fabs(t_sum - sum);
        t_rel_err = t_abs_err / fabs(t_sum);
        if (i != 0 && t_abs_err < abs_err && t_rel_err <= rel_err) break;
        if (200 * pow(2, i + 1) > max_division) break;
        t_sum = sum;
        sum = 0;
    }
    auto finish_time = get_current_time_fenced() - stage1_start_time;
    std::cout<<"Time : "<<to_us(finish_time)<<std::endl;
    cout << "Abs err : " << abs_err << "; rel err : " << rel_err << endl;
    std::cout << std::setprecision(20) <<"Res : "<< sum << std::endl;
    return 0;
}
