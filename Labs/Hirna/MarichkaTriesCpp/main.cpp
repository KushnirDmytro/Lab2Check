#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <mutex>


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


void shuberts(int m, double x1, double x2, double *res){
    double result = 0;
    double sum1 = 0;
    double sum2 = 0;
    for (int i=0; i<m; i++){
        sum1 += i * cos((i + 1) * x1 + 1);
        sum2 += i * cos((i + 1) * x2 + 1);
    }
    result = -(sum1 * sum2);
    *res = result;
}

double integrate_func(double x1, double x2, double y1, double y2, double deltaX, double deltaY, int m, double *res){
    double result = 0;
    for (double i=x1; i<x2; i+=deltaX){
        for (double j=y1; j<y2; j+=deltaY){
            double temp_res;
            shuberts(m, i, j, &temp_res);
            result += temp_res * deltaX * deltaY;
        }
    }
    *res = result;
}


double integrate_func_threads(double x1, double x2, double y1, double y2, double deltaX, double deltaY, int m, double &res, std::mutex& mux){
    double result = 0;
    for (double i=x1; i<x2; i+=deltaX){
        for (double j=y1; j<y2; j+=deltaY){
            double temp_res;
            shuberts(m, i, j, &temp_res);
            result += temp_res * deltaX * deltaY;
        }
    }
    mux.lock();
    res += result;
    mux.unlock();
}



int main(int argc, char* argv[]){
    auto start_time = get_current_time_fenced();

    double x1, x2, y1, y2, deltaX, deltaY, res, rel_prec, abs_prec;
    int m, threads_num;

    x1 = -100.0;
    x2 = 100.0;
    y1 = -100.0;
    y2 = 100.0;
    deltaX = 0.5;
    deltaY = 0.5;
    m = 5;

    threads_num = 4;

    if (argc == 10){

        rel_prec = atof(argv[0]);
        abs_prec = atof(argv[1]);
        threads_num = atoi(argv[2]);
        x1 = atof(argv[5]);
        x2 = atof(argv[6]);
        y1 = atof(argv[7]);
        y2 = atof(argv[8]);
        deltaX = (abs(x2 - x1)) / atof(argv[3]);
        deltaY = (abs(y2 - y1)) / atof(argv[4]);
        m = atoi(argv[9]);

        if (abs_prec < 0.001){
            std::cout<<"Absolute precision is too small." << std::endl;
            return 1;
        }

        if (threads_num == 0){
            std::cout<<"Threads number can not be zero." << std::endl;
            return 1;
        }

        if (deltaX > 0.05){
            std::cout<<"Step for X is too small. Not correct result expected." << std::endl;
            return 1;
        }

        if (deltaY > 0.05){
            std::cout<<"Step for Y is too small. Not correct result expected." << std::endl;
            return 1;
        }

    }

    else {
        std::cout<<"Not enough parameters." << std::endl;
        return 1;
    }

    double final_res = 0;
    double temp_res = 0;

    std::mutex mux;
    std::vector<std::thread> threads;

    double threads_interval = abs(x1 - x2) / threads_num;

    for (int i=0; i < threads_num - 1; i++){
        threads.emplace_back(integrate_func_threads, x1 + threads_interval * i, x1 + threads_interval * (i + 1), y1, y2, deltaX, deltaY, m, std::ref(final_res), std::ref(mux));
    }

    threads.emplace_back(integrate_func_threads, x1 + threads_interval * (threads_num - 1), x2, y1, y2, deltaX, deltaY, m, std::ref(final_res), std::ref(mux));

    for (int i = 0; i < threads_num; ++i) {
        threads[i].join();
    }

    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - start_time;

    std::cout<<final_res<<std::endl;
    std::cout << "Total time: " << to_us(total_time) << std::endl;

    return 0;
}
