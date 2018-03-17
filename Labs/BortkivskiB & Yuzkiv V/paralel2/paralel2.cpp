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


//#define NUMBER_OF_THREADS 1
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

void loopp(double step, double start, double end, double start_x, double end_x, long *step_result, std::mutex &m){
    double dot_sum = 0;
    for(double y=start; y<end; y+=step)
        for(double x=start_x; x<=end_x; x+=step)
            dot_sum += f(x, y);
        
    

    m.lock();
    *step_result += dot_sum;
    m.unlock();    
}


int main(int argc, char* argv[]) {
    
    for(int i=1;i<argc;++i){
        if(strcmp(argv[i], "--help")==0 || strcmp(argv[i],"-h")==0){
            std::cout << "./program min_x max_x min_y max_y number_of_threads absolute_error relative_error" << std::endl;
            return 0;
        }
    }
    
    if(argc != 8){
        std::cerr << "Invalid number of arguments!" << std::endl;
        std::cerr << "Use -h|--help for more info" << std::endl;
        return 1;
    }
    
    
    
    double min_x = std::stod(argv[1]);//-100;//argv[1]
    double max_x = std::stod(argv[2]);//100;//argv[2]
    double min_y = std::stod(argv[3]);//-100;//argv[3]
    double max_y = std::stod(argv[4]);//100;//argv[4]
    
    int number_of_threads = (int)std::stod(argv[5]);//4;//argv[5]
    
    double absolute_error = std::stod(argv[6]);//80;//argv[6]
    double relative_error = std::stod(argv[7]);//0.0001;//argv[7]
    
    if(max_x < min_x){
        std::cerr << "max_x should be greater than min_x" << std::endl;
        return 1;
    }
    
    if(max_y < min_y){
        std::cerr << "max_y should be greater than min_y" << std::endl;
        return 1;
    }
    
    if(number_of_threads < 1 || number_of_threads > 4){
        std::cerr << "1 <= number_of_threads <= 4" << std::endl;
        return 2;
    }
    
    if(absolute_error <= 0){
        std::cerr << "absolute_error should be positive" << std::endl;
        return 3;
    }
    
    if(relative_error > 0.001){
        std::cerr << "relative_error should be less than 0.001" << std::endl;
        return 3;
    }
    
    std::mutex m;
    //for(int number_of_threads=1;number_of_threads<=4;++number_of_threads){
        
    long global_result = 0;
    long division = 125;
    long old_result = 0;
    std::thread ar[number_of_threads];
    auto stage1_start_time = get_current_time_fenced();
    int counter = 0;
    //bool should_start = true;
    //while( fabs(old_result-global_result) > absolute_error || should_start ){
    //    should_start = false;
    while(true){
        if(counter>13){
            //exceeded million divisions
            break;
        }
        ++counter;
		std::cout << '.' << std::flush;
        
        if(old_result != 0){
            if((double)old_result/(double)global_result > 1-relative_error || (double)global_result/(double)old_result > 1-relative_error){
                if(fabs(old_result-global_result) < absolute_error){
                    break;
                }
            }
        }
        //std::cout << "iter" << std::endl;
        division *= 2;
        //std::cout << old_result << " " << global_result << std::endl;
        old_result = global_result;
        
        double step = (max_y-min_y)/(double)division;
        double area = step*step;
        
    
        for(int i=0; i<number_of_threads; ++i){
            //(2*i-t)*(max_y-min_y)/2/t;i*200/number_of_threads-100
            ar[i] = std::thread(loopp, step, (int)((2*i-number_of_threads)*(max_y-min_y)/2/number_of_threads), (int)((2*(i+1)-number_of_threads)*(max_y-min_y)/2/number_of_threads), min_x, max_x, &global_result, std::ref(m));
        }
        
        for(int i=0; i<number_of_threads; ++i){
            ar[i].join();
        }
        
        global_result *= area;
        //std::cout << old_result << " " << global_result << std::endl;
    }
    
    
    auto finish_time = get_current_time_fenced();

    auto total_time = finish_time - stage1_start_time;
    /*
    std::cout << "Number of threads: " << number_of_threads << std::endl;
    std::cout << "Total time: " << to_us(total_time) << std::endl;
    std::cout << "Result: " << global_result << std::endl;
    */
    //std::cout << global_result << std::endl;
	//}
    
    std::ofstream myfile;
    myfile.open("program_result.csv");
    myfile << global_result << "," << fabs(global_result-old_result) << "," << global_result/old_result << "," << to_us(total_time);
    myfile.close();
    return 0;
	
}
