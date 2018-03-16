#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <iomanip>

#define A 20
#define B 0.2
#define C 6.28318530718
int STEPS=500;

using namespace std;


inline std::chrono::steady_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::steady_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D& d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

struct config{
    double abs;
    double rel;
    int thr;
    double x1;
    double x2;
    double y1;
    double y2;
    int max;
};
//
//config readconf(FILE* file){
//    char* line = new char[200];
//    fgets(line, 200, file);
//    char* lines[8];
//    for(int i=0;i<8;i++){
//
//    }
//}
inline double func(const double &x1, const double &x2){
    return -1*A*exp(-1*B*sqrt((x1*x1+x2*x2)/2))-exp((cos(C*x1)+cos(C*x2))/2)+A+exp(1);
}
double de_jong(double x, double y) {//just for fun
    double sum = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x - 16 * j), 6) + pow((y - 16 * i), 6));
        }
    }
    return (double)1 / (0.002 + sum);
}
void integrate(double x1, const double &x2,double y1, const double &y2,const int &steps, double & result) {
    double res = 0;
    double dx = (x2-x1)/steps;
    double dy = (y2-y1)/steps;
    double y1_1 = y1;
    while (x1<x2){
        while(y1<y2) {
            res += func(x1, y1)*dx*dy;
            y1+=dy;
        }
        x1+=dx;
        y1=y1_1;
    }
    result = res;
}


int main(int argc, char** argv) {

    config conf;
    if(argc==9){
        conf.abs=atof(argv[1]);
        conf.rel=atof(argv[2]);
        conf.thr=atoi(argv[3]);
        conf.x1=atof(argv[4]);
        conf.x2=atof(argv[5]);
        conf.y1=atof(argv[6]);
        conf.y2=atof(argv[7]);
        conf.max=atoi(argv[8]);
    }
    else{
        cerr<<"bad args\n";
        return 1;
    }
    if(conf.abs<0||conf.abs>100000||conf.rel<0||conf.rel>1) {
        cerr << "Wrong error\n";
        return 1;
    }
    if(conf.thr<1){
        cerr<<"No threads to run\n";
        return 1;
    }
    if(conf.x1>=conf.x2||conf.y1>=conf.y2){
        cerr<<"Wrong borders\n";
        return 1;
    }
    if(conf.max<1){
        cerr<<"Not enough steps\n";
        return 1;
    }
    auto starttime = get_current_time_fenced();
    double prev,sum=0;
    int iter=0;
    do {
        prev=sum;
        sum=0;
        double results[conf.thr] = {0};
        vector<thread> threads;
        for (int i = 0; i < conf.thr; ++i) {
            threads.emplace_back(integrate, conf.x1 + i * (conf.x2 - conf.x1) / conf.thr,
                                 conf.x1 + (i + 1) * (conf.x2 - conf.x1) / conf.thr,
                                 conf.y1, conf.y2, STEPS/conf.thr, ref(results[i]));
        }
        for (int i = 0; i < conf.thr; ++i) {
            threads[i].join();
            sum += results[i];
        }
        //cout<<setprecision(20)<<sum<<endl<<STEPS<<endl;
        STEPS*=2;
        //threads.clear();
        iter++;
    }while(iter<conf.max&&abs(sum-prev)>conf.abs&&abs(1-(sum/prev))>conf.rel);
    auto time = to_us(get_current_time_fenced()-starttime);
    cout<<setprecision(20)<<sum<<" "<<time<<endl;
    return 0;
}