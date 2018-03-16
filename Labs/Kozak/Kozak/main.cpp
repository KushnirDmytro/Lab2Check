#include <iostream>
#include <chrono>
#include <atomic>
#include <fstream>
#include <math.h>
#include <thread>
#include <mutex>
using std::mutex;

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

double func(double x1, double x2){
    double a = 20;
    double b = 0.2;
    double c = 2 * M_PI;
    return -a * exp(-b * sqrt(1.0/2 * (x1*x1 + x2*x2))) - exp(1.0/2 * (cos(c * x1) + cos(c * x2))) + a + exp(1);
}

double integral(double x1, double x2, double y1, double y2, double step, double &res, mutex &mtx) {
    double sum1 = abs(x1) + abs(x2);
    double sum2 = abs(y1) + abs(y2);
    double integral = 0;
    for(double x = x1; x <= x2; x += step){
        for(double y = y1; y <= y2; y += step){
            integral += func(x,y)*step*step;
        }
    }
    mtx.lock();
    res+= integral;
    mtx.unlock();
    return integral;
}

int main(int argc, char* argv[]) {
    mutex mtx;
    int thr = 8;
    double x1 = -100;
    double x2 = 100;
    double y1 = -100;
    double y2 = 100;
    double kvad = 0.1;

    double res[thr];
    std::thread threads[thr];

    double d1 = (abs(x1) + abs(x2))/thr;
    double x_1 = x1;
    for (int i = 0; i < thr; ++i) {
        res[i] = 0;
    }
    auto start = get_current_time_fenced();
    for (int j = 0; j < thr; ++j) {
        if (j == (thr - 1)) {
            x_1 = x1 - d1;
        }
        threads[j] = std::thread(integral, x_1,x_1+d1,y1,y2,kvad, std::ref(res[j]), std::ref(mtx));
        x_1 += d1;
    }
    for (auto &thread: threads){
        thread.join();
    }
    auto end = get_current_time_fenced();
    auto result = end - start;
    double final = 0;
    for (double &ri:res) {
        final += ri;
    }
    std::cout << final << std::endl;
    std::cout << to_us(result)<< std::endl;
    return 0;
}
/*
 *     int thr = atoi(argv[1]);
    double x1 = (double)atoi(argv[2]);
 */