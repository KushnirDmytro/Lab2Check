//
// Created by Rostyk Popov on 3/12/18.
//


#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <atomic>
#include <chrono>

using  namespace std;
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

double integrate(int M, int N, int n, double maxX, double minX, double maxY, double minY){
    vector<double> vector1 = parse_cfg();
    double deltaX = (maxX - minX)/n;
    double deltaY = (maxY - minY)/n;
    double result = 0;
    for (int i = 1; i < M; ++i) {
        double strihX = (minX + deltaX * i + minX + deltaX * (i - 1)) / 2;
        for (int j = 1; j < N; ++j) {
            double strihY = (minY + deltaY * j + minY + deltaY * (j - 1)) / 2;
            result += func(strihX, strihY) * deltaX * deltaY;
        }
    }
    return result;
}

double calculate_abs(double res1, double res2){
    return res1 - res2;
}
double calculate_rel(double res1, double res2){
    return(res1 - res2)/res1;
}
int main(){
    double res;
    double res2;
    vector<double> vector1 = parse_cfg();
    double abs = vector1[5];
    double rel = vector1[6];
    double maxX = vector1[0], minX = vector1[1], maxY = vector1[2], minY = vector1[3];
    int divNumber = 200;
    auto start = get_current_time_fenced();
    res = integrate(divNumber, divNumber, divNumber, maxX, minX, maxY, minY);
    divNumber *= 2;
    res2 = integrate(divNumber, divNumber, divNumber, maxX, minX, maxY, minY);
    while (calculate_abs(res, res2) > abs && calculate_rel(res, res2) > rel){
        divNumber *= 2;
        res = res2;
        res2 = integrate(divNumber, divNumber, divNumber, maxX, minX, maxY, minY);
    }
    auto finish = get_current_time_fenced();
    auto total_time = finish - start;

    ofstream myfile;
    myfile.open("results_int.txt", ios::app);
    myfile<<res2<<" "<<to_us(total_time)<<" "<<abs<<" "<<rel<<" ";
    myfile.close();
}
