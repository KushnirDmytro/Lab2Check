//
// Created by Rostyk Popov on 3/12/18.
//

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

using  namespace std;

double finalRes = 0;
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

vector<double> parse_cfg(){
    string line;
    ifstream file;
    vector<double> vector1;
    file.open("cfg.txt");
    int output;
    if(file.is_open()){
        while(!file.eof()){
            file >> output;
            vector1.push_back(output);
        }
    }

    return vector1;
}

double func(double x1, double x2, double a = 20, double b = 0.2, double c = 2*M_PI){
    return -a*exp(-b * sqrt((pow(x1, 2) + pow(x2, 2)) / 2)) - exp((cos(x1*c) + cos(x2*c)) / 2) + a + exp(1);
}

void integrate(int M, int N, int n, int start, mutex& m, double maxX, double minX, double maxY, double minY){
    int start_x = 1;
    double deltaX = (maxX - minX)/n;
    double deltaY = (maxY - minY)/n;
    double result = 0;
    for (int i = start_x; i < M; ++i) {
        double strihX = (minX + deltaX * i + minX + deltaX * (i - 1)) / 2;
        for (int j = start; j < N; ++j) {
            double strihY = (minY + deltaY * j + minY + deltaY * (j - 1)) / 2;
            result += func(strihX, strihY) * deltaX * deltaY;
        }
    }
    m.lock();
    finalRes += result;
    m.unlock();
}
void threads(int Nthreads, int divisionNum){
    vector<double> vector1 = parse_cfg();
    double maxX = vector1[0], minX = vector1[1], maxY = vector1[2], minY = vector1[3];
    mutex m;
    int add = divisionNum/Nthreads;
    int end =  divisionNum/Nthreads;
    int start = 0;
    int start_x = 1;
    vector <thread> threads;
    for(int i = 0; i<Nthreads; ++i){
        threads.emplace_back(thread(integrate, divisionNum, end, divisionNum, start + 1, ref(m), maxX, minX, maxY, minY));
        start = end;
        end += add;
    }
    for(auto& thread:threads){
        thread.join();
    }
}
double calculate_abs(double res1, double res2){
    return res1 - res2;
}
double calculate_rel(double res1, double res2){
    return(res1 - res2)/res1;
}

double parseResult(){
    double res1;
    double res;
    vector<double> vector1 = parse_cfg();
    int Nthreads = vector1[4];
    double abs = vector1[5];
    double rel = vector1[6];
    int divNumber = 200;
    threads(Nthreads, divNumber);
    res1 = finalRes;
    finalRes = 0;
    divNumber *= 2;
    threads(Nthreads, divNumber);
    res = finalRes;
    while (calculate_abs(res1, res) > abs && calculate_rel(res1, res) > rel){
        res1 = res;
        finalRes = 0;
        divNumber *= 2;
        threads(Nthreads, divNumber);
        res = finalRes;
    }
    finalRes = 0;
    return res;
}
int main(){
    double res;
    vector<double> vector1 = parse_cfg();
    double abs = vector1[5];
    double rel = vector1[6];
    auto start = get_current_time_fenced();
    res = parseResult();
    auto finish = get_current_time_fenced();
    auto total_time = finish - start;
    ofstream myfile;
    myfile.open("results_th.txt", ios::app);
    myfile<<res<<" "<<to_us(total_time)<<" "<<abs<<" "<<rel<<" ";
    myfile.close();
    return 0;
}
