#include <iostream>
#include <thread>
#include <cmath>
#include <chrono>
#include <atomic>
#include <mutex>
#include <windows.h>
#include "cmake-build-debug/configuration.h"
#include <fstream>
#include <vector>
#include "cmake-build-debug/results.h"


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

double function(double x, double y);
//Ця функція дає точніший результат, але тільки коли проміжок націло ділиться  на кількість потоків, похибка приблизно дорівнює 0.7 * 10^(-7)
void integral(double x_start, double  x_stop,std :: mutex& m, config::Configuration &conf);

void printResults(config::Configuration& configuration);
void configure(config::Configuration &configuration);


int main() {

    config::Configuration conf;
    std::mutex ios_mutex;
    try {
        configure(conf);
    }catch (...){
        std::cout<<"FAILURE DURING CONFIGURATION";
        return 1;
    }
    std::cout<<conf;
    std::vector<double> intervals;

    int tmp;

    if((conf.getInterval_x_end()-conf.getInterval_x_start())%conf.getAmount_of_threads() == 0){
        tmp = (conf.getInterval_x_end()-conf.getInterval_x_start())/conf.getAmount_of_threads();
        for(int i =0; i<conf.getAmount_of_threads(); i++){
            intervals.push_back(conf.getInterval_x_start()+(i*tmp));
        }

    } else{
        tmp = (conf.getInterval_x_end()-conf.getInterval_x_start())/conf.getAmount_of_threads();
        for(int i =0; i<conf.getAmount_of_threads(); i++){
            intervals.push_back(conf.getInterval_x_start()+(i*tmp));
        }
        intervals.push_back(conf.getInterval_x_end());
    }


    bool  good_res = false;
    std::chrono::duration<double> total_time;
    std::chrono::_V2::system_clock::time_point start_time;
    std::chrono::_V2::system_clock::time_point finish_time;
    start_time = get_current_time_fenced();
    do {
        std::thread *pool = new std::thread[conf.getAmount_of_threads()];



        for (int i = 0; i < conf.getAmount_of_threads(); i++) {
            if(intervals.size()==conf.getAmount_of_threads()&& i==conf.getAmount_of_threads()-1){
                pool[i] = std::thread(integral, intervals[i],conf.getInterval_x_end(), std::ref(ios_mutex), std::ref(conf));
            } else {
                pool[i] = std::thread(integral, intervals[i], intervals[i + 1], std::ref(ios_mutex), std::ref(conf));
            }
        }
        for (int i = 0; i < conf.getAmount_of_threads(); i++) {
            pool[i].join();
        }


        if(results.turn == 1) {
            results.result_turn_1 = results.res;
            results.res = 0;
            conf.setN(conf.getN()*2);
            results.turn = 2;
        } else{
            results.result_turn_2 = results.res;
            results.res = 0;

            if(results.result_turn_1-results.result_turn_1 <= fabs(results.result_turn_1 - results.result_turn_1) &&
               fabs(results.result_turn_1-results.result_turn_2)/results.result_turn_1  <= (results.result_turn_2 / results.result_turn_1)/results.result_turn_2 &&
               (results.result_turn_1 / results.result_turn_2)/results.result_turn_1 < conf.getRelative_mistake()&&
               fabs(results.result_turn_1 - results.result_turn_1) <= conf.getAbsolute_mistake()){
                good_res =  true;
                finish_time = get_current_time_fenced();
            } else{

                results.turn = 1;
                results.result_turn_1 = 0;
                results.result_turn_2 = 0;
                results.res = 0;
                conf.setN(conf.getN()*2);
            }
        }

    }while (!good_res);

    total_time = finish_time - start_time;
    results.res = results.result_turn_1;
    results.relative_mistake = fabs(results.result_turn_1 - results.result_turn_2) / results.result_turn_1;
    results.absolute_mistake = fabs(results.result_turn_1 - results.result_turn_2);
    results.computation_time_us = to_us(total_time);
    results.computation_time_sec = total_time.count();



    printResults(conf);
    // time_t end_time = std::chrono::system_clock::to_time_t(end);



    return 0;
}
void configure(config::Configuration &configuration){

    std::ifstream fout("config.txt");
    if (!fout.is_open()) {
        std::ofstream confout("config.txt");
        confout <<configuration.getAbsolute_mistake_token() <<":{1}" << std::endl;
        confout <<configuration.getRelative_mistake_token() <<":{0.001}" << std::endl;
        confout <<configuration.getAmount_of_threads_token()<<":{4}" << std::endl;
        confout <<configuration.getInterval_x_start_token() <<":{-50}" << std::endl;
        confout <<configuration.getInterval_x_end_token()   <<":{50}" << std::endl;
        confout <<configuration.getInterval_y_start_token() <<":{-50}" << std::endl;
        confout <<configuration.getInterval_y_end_token() <<":{50}" << std::endl;
        confout.close();
        std::ifstream fout("config.txt");
    }

    const int buf_size = 50;
    char buf[buf_size];
    std::string input_string;

    double n=0;
    while (fout.getline(buf, 50)){
        input_string.clear();
        input_string.append(buf);
        if(input_string.find(configuration.getAbsolute_mistake_token())==0){

            n = configuration.parseValue(buf,buf_size);
            if(n <= 0){
                std::cout<<"Mistake can`t be less or equal than zero\n";
                throw EIO;

            }

            configuration.setAbsolute_mistake(n);
            configuration.ok();


        } else if(input_string.find(configuration.getRelative_mistake_token())==0){

            n = configuration.parseValue(buf,buf_size);

            if(n <= 0){
                std::cout<<"Mistake can`t be less or equal than zero\n";
                throw EIO;
            }
            configuration.setRelative_mistake(n);
            configuration.ok();

        }else if(input_string.find(configuration.getAmount_of_threads_token())==0){
            n = configuration.parseValue(buf,buf_size);
            if(n<=0){
                std::cout<<"Amount of threads can`t be <0\n ";
                throw EIO;
            }
            configuration.setAmount_of_threads((int)n);
            configuration.ok();



        }else if(input_string.find(configuration.getInterval_x_start_token())==0){

            n = configuration.parseValue(buf,buf_size);
            configuration.setInterval_x_start((int)n);
            configuration.ok();



        }else if(input_string.find(configuration.getInterval_x_end_token())==0){

            n = configuration.parseValue(buf,buf_size);
            configuration.setInterval_x_end((int)n);
            configuration.ok();

        }else if(input_string.find(configuration.getInterval_y_start_token())==0){

            n = configuration.parseValue(buf,buf_size);
            configuration.setInterval_y_start((int)n);
            configuration.ok();

        }else if(input_string.find(configuration.getInterval_y_end_token())==0){
            n = configuration.parseValue(buf,buf_size);
            configuration.setInterval_y_end((int)n);
            configuration.ok();
        }
    }
    if(configuration.getFields_initialized()!=7){
        throw EIO;
    }




}
double function(double x, double y){

    double  total_res = 0;

    double  local_res = 0;
    for(double i = -2; i<=2; i++){
        for(double j =-2; j<=2; j++){
            local_res +=1/(5*(i+2) + j + 3 + pow((x - 16*j),6)+ pow((y - 16*i),6) );
        }
        total_res +=local_res;
        local_res = 0;
    }


    total_res = total_res + 0.002;

    total_res = pow(total_res, -1);
    return total_res;

}

//Ця функція дає точніший результат, але тільки коли проміжок націло ділиться  на кількість потоків, похибка приблизно дорівнює 0.7 * 10^(-7)б при n = 10.000
void integral(double x_start,double  x_stop,std :: mutex& m, config::Configuration& conf){

    m.lock();
    double delta_x = (conf.getInterval_x_end()-conf.getInterval_x_start())/conf.getN();
    double detla_y =(conf.getInterval_y_end() - conf.getInterval_y_start())/conf.getN();

    double N = (x_stop-x_start)*(conf.getN()/100);
    double M = conf.getN();
    double y_start = conf.getInterval_y_start();
    m.unlock();
    double x;
    double y;
    double total_res=0;
    double lockal_res =0;

    for(double i =1; i <= N; i++){
        x = (x_start + i*delta_x + x_start + (i-1) *delta_x)/2;
        for(double j=1; j <= M; j++){
            y = (y_start + j*detla_y + y_start + (j-1)*detla_y)/2;
            lockal_res += function(x,y)*delta_x*detla_y;
        }
        total_res +=lockal_res;
        lockal_res = 0;
    }
    m.lock();
    results.res+=total_res;
    m.unlock();
}


void printResults(config::Configuration& configuration){
    std::cout.precision(15);
    std::cout << " Amount of results " << results.res << std::endl;
    std::cout << " Amount of threads " << configuration.getAmount_of_threads() << std::endl;
    std::cout << " Relative mistake  " << results.relative_mistake << std::endl;
    std::cout << " Absolute mistake  " << results.absolute_mistake << std::endl;
    std::cout << "Total time: " << results.computation_time_us << " us "<<std::endl;
    std::cout << "elapsed time: " << results.computation_time_sec <<" sec "<< std::endl;


    std::ofstream fout("results.txt", std::ios_base::app);
    fout << "Result : " << results.res << std::endl;
    fout << "Amount of threads " << configuration.getAmount_of_threads() << std::endl;
    fout << "Relative mistake  " << results.relative_mistake << std::endl;
    fout << "Absolute mistake  " << results.absolute_mistake << std::endl;
    fout << "Total time us : " << results.computation_time_us << " us "<<std::endl;
    fout << "Total time sec : " << results.computation_time_sec <<" sec "<< std::endl;
    fout<<"//----------------------------------------------------------------//\n"<<std::endl;

}