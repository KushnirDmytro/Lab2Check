#include <iostream>
#include<thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
#include <cmath>

double func3(int x, int y){
	double f = -1;
	double f1, f2 = 0;
	int m = 5;
	for(int i = 0; i < m; i++){
		f1 += i*(std::cos((i + 1) * x + 1));
		f2 += i*(std::cos((i + 1) * y + 1));
	}
	f *= f1 * f2;
	return f;
}


double integral(int x, int y){
	int nb, mb = 1000;
	double sum = 0;
	int n = 0;
	int p1, p2 = 0;
	for(int s1 = -50; s1 < mb; s1++){
		for(int s2 = -50; s2 < nb; s2++){
			double n1 = (p1 + s1) / 2;
			double n2 = (p2 + s2) / 2;
			double d1 = s1 - p1;
			double d2 = s2 - p2;
			n++;
			sum += d1 * d2 * func3(n1, n2);;
			p1 = s1;
			p2 = s2;
		}
	}
	std::cout << "adding" << sum << std::endl;
	return sum;
}

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
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}


int main(int argc, char* argv[]) {
	if (argc == 1){
		std::cout << "Arguments needed(strict order!): user's absolute... "
	}
	else if(argc != 6){
		std::cout << "Not enough arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	else{
		double ba = argv[1];
		double bv = argv[2];
		int nthreads = argv[3];
		int n, m = argv[4], argv[5];
	}
	int nthreads = 2;
	std::vector<std::thread> threads;
	std::mutex m;
	double expl = -5.045849736;
	auto stage1_start_time = get_current_time_fenced();
	for(int i = 0; i < nthreads; ++i){
		//threads.emplace_back(integral, n, m, std :: ref (m) );
	}
	double res = integral(n, m);
	double errab = abs(expl - res);
	double errre = errab/expl;
	std::cout << "fin " << res << ' ' << errab << ' ' << errre << std::endl;
		
    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - stage1_start_time;
    std::cout << "Total time: " << to_us(total_time) << " sec" << std::endl;

}
