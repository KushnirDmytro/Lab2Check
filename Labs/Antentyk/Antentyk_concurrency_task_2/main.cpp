#include <iostream>
#include <string>
#include <chrono>
#include <atomic>
#include <iomanip>


#include "options/options.h"
#include "integral/integral.h"


inline std::chrono::high_resolution_clock::time_point get_current_time_fenced(){
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}


template<class D>
inline long long to_us(const D& d){
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}


int main(int argc, char ** argv){
    std::string config_file_name = options :: retrieve(argc, argv);

    settings :: SettingsHolder settings = 
        settings :: SettingsRetriever(config_file_name).retrieve();

    integral :: Integrator integrator{settings};

    std::chrono::high_resolution_clock::time_point start, end;

    start = get_current_time_fenced();
    integral :: Result result = integrator.run();
    end = get_current_time_fenced();

    std::cout<<result<<std::endl;
    std::cout<<to_us(end - start)<<std::endl;


    exit(EXIT_SUCCESS);
}