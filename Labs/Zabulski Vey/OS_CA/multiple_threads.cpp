#include <iostream>
#include <chrono>
#include <atomic>
#include<map>
#include<string>
#include<vector>
#include<cmath>
#include <mutex>
#include <thread>

using std::cout;
using std::cin;
using std::map;
using std::endl;
using std::string;
using std::thread;
using std::mutex;
using std::ref;
using std::vector;

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


double f(double x, double y, int m){

}


void worker(double xmin, double xmax, double ymin, double ymax, int parts, int m, double& result, mutex& mut){
    double res = 0;
    double dx = (xmax - xmin) / parts;
    double dy = (ymax - ymin) / parts;
    double x, y;

    for(int i = 0; i < parts; i++) {
        for(int j = 0; j < parts; j++){  
            double tmp1 = 0, tmp2 = 0;
            x = xmin + dx * i;
            y = ymin + dy * j;
            for(int k = 1; k <= m; k++){
                tmp1 += k * cos((k + 1) * x + 1);
            }
            for(int k = 1; k <= m; k++){
                tmp2 += k * cos((k + 1) * y + 1);
            }
            // res += x * dx;
            res += (-tmp1 * tmp2) * dx;
        }
    }
    res = res * dy;

    mut.lock();
    // cout<<"thread info: "<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<" "<<parts<<endl;    
    // cout<<"thread result: "<<res<<endl;
    result += res;
    mut.unlock();

}


int main(int argc, char* argv[]){
    auto start_time = get_current_time_fenced();

    map<string, double> arguments = {{"-xmin", -100.0 },
                                 {"-xmax", 100.0 },
                                 {"-ymin", -100.0 },
                                 {"-ymax", 100.0 },
                                 {"-relerr", 0.001},
                                 {"-maxiter", 8.0},
                                 {"-abserr", 0.2},
                                 {"-m", 5},
                                 {"-parts", 200},
                                 {"-maxparts", 200000},
                                 {"-threads", 4}};
    for(int i = 0; i<argc - 1; i++){
        auto pos = arguments.find(argv[i]);
        if(pos != arguments.end()){
            pos->second = atoi(argv[i + 1]);
        }
    }

    int iteration = 1;
    double res;
    double xmax = arguments["-xmax"];
    double xmin = arguments["-xmin"];
    double ymax = arguments["-ymax"];
    double ymin = arguments["-ymin"];

    double abs_err = arguments["-abserr"];
    double rel_err = arguments["-relerr"];

    int threads_amount = (int)arguments["-threads"];
    int m = (int)arguments["-m"];
    int max_iterations = (int)arguments["-maxiter"];
    int parts = (int)arguments["-parts"];
    int max_parts = (int)arguments["-maxparts"];
    int new_parts;



    // cout<<"Max iter(tmp) "<<max_iterations<<endl;
    // cout<<"Threads(tmp) "<<threads_amount<<endl;

    
    int range = (xmax - xmin) / threads_amount;
    mutex mut;

    new_parts = parts;
    double prev, curr_abs_err, curr_rel_err;
    for(int iteration = 1; iteration <= max_iterations; iteration++){
        res = 0;
        cout<<"Iteration " << iteration << endl;




        vector<thread>threads;
        int parts_by_thread = new_parts / threads_amount;
        for(int i = 0; i < threads_amount - 1; i++){
            threads.emplace_back(worker, xmin + range*i, xmin + range*(i+1), ymin, ymax, parts_by_thread, m, ref(res), ref(mut));
        }
        int rest = new_parts - (parts_by_thread * threads_amount);
        threads.emplace_back(worker, xmin+range*(threads_amount-1), xmax, ymin, ymax, parts_by_thread + rest, m, ref(res), ref(mut));


        for(int i = 0; i < threads_amount; i++){
            threads[i].join();
        }

        
        cout << res << endl;
        curr_abs_err = fabs(prev - res);
        curr_rel_err = fabs(prev - res) / fabs(prev);
        if(iteration != 1 && curr_abs_err < abs_err && curr_rel_err <= rel_err){
            break;
        }
        prev = res;
        new_parts = 2 * new_parts;
        if (new_parts >= max_parts){
            break;
        }
    }


    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - start_time;
    cout<< "Abs err: " << curr_abs_err << endl << "Rel err: " << curr_rel_err << endl; 
    cout << "Total time: " << to_us(total_time) << endl;
    return 0;
}
//g++ -std=c++14 -O3 -ffast-math -pthread multiple_threads.cpp -o multiple_threads && ./multiple_threads -xmin 0 -ymin 0 -parts 100 -threads 4
//g++ -std=c++14 -O3 -ffast-math -pthread single_thread.cpp -o single_thread && ./single_thread