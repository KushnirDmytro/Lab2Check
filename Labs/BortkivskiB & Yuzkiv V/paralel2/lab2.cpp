#include <iostream>
#include <chrono>
#include <atomic>
#include <cmath>
#include <mutex>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <math.h>
#include <fstream>


#define PI 3.14159265

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

double f(double x1, double x2){
    return -20*exp(-0.2*sqrt(0.5*(pow(x1,2)+pow(x2,2))))-exp(0.5*(cos(2*PI*x2)+cos(2*PI*x1)))+20+exp(1);
}

void loopp(double step, double start, double end, double start_x, double end_x, long *step_result){
    double dot_sum = 0;
    for(double y=start; y<end; y+=step)
        for(double x=start_x; x<=end_x; x+=step)
            dot_sum += f(x, y);
        
    *step_result += dot_sum;
    
}

int main(int argc, char* argv[]) {
    
    for(int i=1;i<argc;++i){
        if(strcmp(argv[i], "--help")==0 || strcmp(argv[i],"-h")==0){
            std::cout << "./program min_x max_x min_y max_y absolute_error relative_error" << std::endl;
            return 0;
        }
    }
    
    if(argc != 7){
        std::cerr << "Invalid number of arguments!" << std::endl;
        std::cerr << "Use -h|--help for more info" << std::endl;
        return 1;
    }
    
    double min_x = std::stod(argv[1]);
    double max_x = std::stod(argv[2]);
    double min_y = std::stod(argv[3]);
    double max_y = std::stod(argv[4]);
    
    double absolute_error = std::stod(argv[5]);
    double relative_error = std::stod(argv[6]);
    
    if(max_x < min_x){
        std::cerr << "max_x should be greater than min_x" << std::endl;
        return 1;
    }
    
    if(max_y < min_y){
        std::cerr << "max_y should be greater than min_y" << std::endl;
        return 1;
    }

    
    if(absolute_error <= 0){
        std::cerr << "absolute_error should be positive" << std::endl;
        return 3;
    }
    
    if(relative_error > 0.001){
        std::cerr << "relative_error should be less than 0.001" << std::endl;
        return 3;
    }
            
    long global_result = 0;
    long division = 125;
    long old_result = 0;
    auto stage1_start_time = get_current_time_fenced();
    int counter = 0;
   
    while(true){
        if(counter>13){
            //exceeded million divisions
            break;
        }
        ++counter;
        
        if(old_result != 0){
            if((double)old_result/(double)global_result > 1-relative_error || (double)global_result/(double)old_result > 1-relative_error){
                if(fabs(old_result-global_result) < absolute_error){
                    break;
                }
            }
        }
       
        division *= 2;
        old_result = global_result;
        
        double step = (max_y-min_y)/(double)division;
        double area = step*step;
		
        loopp(step, min_y, max_y, min_x, max_x, &global_result);
        global_result *= area;
    }
    
    
    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - stage1_start_time;
    
    std::ofstream myfile;
    myfile.open("program_result.csv");
    myfile << global_result << "," << fabs(global_result-old_result) << "," << global_result/old_result << "," << to_us(total_time);
    myfile.close();
    return 0;
	
}