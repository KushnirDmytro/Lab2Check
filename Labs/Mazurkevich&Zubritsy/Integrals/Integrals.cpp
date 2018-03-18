#include <iostream>
#include <chrono>
#include <atomic>
#include "cmath"
#include <thread>
#include <mutex>
#include <vector>
#define PI 3.14159265

using std::cout;
using std::endl;
using std::ref;
using std::vector;
using std::thread;
using std::mutex;

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

double func(double x, double y){
    double a = 20.0;
    double b = 0.2;
    double c = 2 * PI;
    double result = -a * exp(-b * sqrt((1.0/2)*(x*x + y*y))) - exp((1.0/2)*(cos(c*x) + cos(c*y))) + a + exp(1);
    return result;
}

double integration(double x1, double x2, double y1, double y2){
    return func((x1+x2)/2, (y1+y2)/2) * (x2-x1) * (y2-y1);
}

void threadfunc(double x1, double x2, double y1, double y2, double &rozb, long double &result, mutex &mtx){
    long double counter = 0;
    for(double i = x1; i < x2; i+=rozb){
        for(double k = y1; k < y2; k+=rozb){
            double integ = integration(i-rozb, i, k-rozb, k);
            counter += integ;
        }
    }
    mtx.lock();
    result += counter;
    mtx.unlock();
}

int main(int argc, char *argv[]) {
    if(argc < 2){cout<< "Too few arguments" << endl; return 1;}
    double absolute = (double) atoi(argv[1]);
    double vidn = (double) atoi(argv[2]);
    int threads = atoi(argv[3]);
    double x_axis[2] = {(double) atoi(argv[4]), (double)atoi(argv[5])};
    double y_axis[2] = {(double) atoi(argv[6]), (double)atoi(argv[7])};
    long double result = 0;
    mutex mtx;
    double rozb = 1.0;
    thread myThreads[threads];
    auto start_time = get_current_time_fenced();
    double part = (std::abs(x_axis[0]) + std::abs(x_axis[1]))/threads;
    double parts = x_axis[0];
    for (int i = 0; i < threads; i++) {
        myThreads[i] = std::move(thread(threadfunc, parts + 1, parts + part, y_axis[0] + 1, y_axis[1], ref(rozb),
                                        ref(result), ref(mtx)));
        parts +=part;
    }
    for (int i = 0; i < threads; i++) {
        myThreads[i].join();
    }
    long double preresult = result/2;
    while(result - preresult > absolute &&preresult/result > vidn) {
        preresult = result;
        result = 0;
        part = (std::abs(x_axis[0]) + std::abs(x_axis[1]))/threads;
        parts = x_axis[0];
        rozb = rozb/2;
        for (int i = 0; i < threads; i++) {
            myThreads[i] = std::move(thread(threadfunc, parts + 1, parts + part, y_axis[0] + 1, y_axis[1], ref(rozb),
                                            ref(result), ref(mtx)));
            parts +=part;
        }
        for (int i = 0; i < threads; i++) {
            myThreads[i].join();
        }
    }
    auto finish_time = get_current_time_fenced();
	cout << "Result: "<<result << endl;
    cout << "Time: "<< to_us(finish_time - start_time) << endl;
	cout << "Absolute Error: " << result - preresult <<endl;
	cout << "Relative Error: " << preresult/result <<endl;
    return 0;
}
