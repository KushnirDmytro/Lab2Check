#include <iostream>
#include <math.h>
#include <chrono>
#include <atomic>
#include <thread>

/* NOT MY CODE */

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

/*(-________-)*/


double subfunct(double x, int m){
    double sum = 0;
    for(int i = 1; i <= m; ++i){
        double res = i * cos( (i + 1) * x + 1);
        sum = sum + res;
    }
    return sum;
}

double shubert(double x1, double x2, int constm){
    return -1 * subfunct(x1, constm) * subfunct(x2, constm);
}

int looop(double* results, double start, double end, int constm, double xstep, double ystep, double sy, double ey){
    double secondsum = 0;
    for (double x = start; x < end; x = x + xstep) {
        for (double y = sy; y <= ey; y = y + ystep) {
            secondsum = secondsum + (xstep * ystep * shubert(x, y, constm));

        }
    }
    results[0] = secondsum;
}

int main(int argc, char* argv[]) {
	/*
	if (argc < 8){
		std::cout << "Too few arguments!" << std::endl;
		return 1;
	}
	
	double ABSMISTAKE = std::stod(argv[1]); //Absolute mistake
	std::cout << ABSMISTAKE << std::endl;
	double RELMISTAKE = std::stod(argv[2]); //Relative mistake
	int numofthr = (int)std::stod(argv[3]); // Number of threads
	std::cout << numofthr << std::endl;
	double SX = std::stod(argv[4]); //Start interval of X
	double EX = std::stod(argv[5]); //End interval of X
	double SY = std::stod(argv[6]); //Start interval of Y
	double EY = std::stod(argv[7]); //End interval of Y
	std::cout << SX << std::endl;
	std::cout << EX << std::endl;
	std::cout << SY << std::endl;
	std::cout << EY << std::endl;
	*/
	int m = 5; // constans value m == 5
	double sx = -100;
	double ex = 100;
	double sy = -100;
	double ey = 100;
	double absmistake = 0.1;
	int numofthr = 3;
	
	
	int step = 125;
	double xstep = 0;
	double ystep = 0;
	double lx = abs(sx - ex);
	double ly = abs(sy - ey);
	
    auto start_time = get_current_time_fenced();
    double firstsum = 0;
    double secondsum = 1;

    while (abs(secondsum - firstsum) >= absmistake) {
        step = step * 2;
        xstep = lx / step;
        ystep = ly / step;
        firstsum = secondsum;
        secondsum = 0;

        double results[numofthr];
        std::thread threads[numofthr];
        
        for (int thr = 0; thr < numofthr; thr++){
            threads[thr] = std::move(std::thread(looop, &results[thr], sx + (lx * thr / numofthr), sx + (lx * (thr + 1) / numofthr), m, xstep, ystep, sy, ey));
        }
        for (int nthr = 0; nthr < numofthr; nthr++){
            threads[nthr].join();
        }
        
        for (int jj = 0; jj < numofthr; jj++){
            secondsum = secondsum + results[jj];
            //std::cout << "Result of thread " << jj + 1 << " is " << results[jj] << std::endl;
        }

        std::cout << step << " Integral is " << secondsum << std::endl;
    }
	auto end_time = get_current_time_fenced();
    auto total_time = end_time - start_time;
    std::cout << "EndIntegral is " << secondsum << std::endl;
	std::cout << "Total time: " << to_us(total_time) << std::endl;
    return 0;

}
