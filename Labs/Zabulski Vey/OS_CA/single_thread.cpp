#include <iostream>
#include <chrono>
#include <atomic>
#include<map>
#include<string>
#include<vector>
#include<cmath>

using std::cout;
using std::cin;
using std::map;
using std::endl;
using std::string;

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
    double res = 0, tmp = 0;
    for(int i = 1; i <= m; i++){
       tmp += i * cos((i + 1) * x + 1);
    }
    for(int i = 1; i <= m; i++){
       res += i * cos((i + 1) * y + 1);
    }
    return -res * tmp;
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
                                 {"-maxparts", 100000}};
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

    int threads = (int)arguments["-threads"];
    int m = (int)arguments["-m"];
    int max_iterations = (int)arguments["-maxiter"];
    int parts = (int)arguments["-parts"];
    int max_parts = (int)arguments["-maxparts"];
    int new_parts;

    new_parts = parts;
    double prev, curr_abs_err, curr_rel_err;
    for(int iteration = 1; iteration <= max_iterations; iteration++){
        res = 0;
        cout<<"Iteration " << iteration << endl;
        double dx = (xmax - xmin) / new_parts;
        double dy = (ymax - ymin) / new_parts;

        for(int i = 0; i < new_parts; i++) {
            for(int j = 0; j < new_parts; j++){
                res += f(xmin + dx * i, ymin + dy * j, m) * dx;
            }
        }
    res = res * dy;
    
    
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