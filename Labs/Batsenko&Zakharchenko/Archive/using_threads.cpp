#include <iostream>
#include <stdlib.h>

#include <math.h>

#include <chrono>
#include <atomic>

#include <mutex>
#include <thread>

#include <vector>

#define M_PI 3.14159265358979323846

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

double function(int c, double x1, double x2, int a1, int a2){
	double sum_sq = (pow(x1 - a1, 2) + pow(x2 - a2, 2));
	double expon = exp(-1.0/M_PI * sum_sq);
	return c * expon * cos(M_PI*sum_sq);
}


void worker( double& count, double start_x, double finish_x, double start_y, double finish_y,  int m, int c[], int a1[], int a2[], double delta,  std::mutex& mu){
	double result = 0, small_res = 0, sum_sq, expon;
	for(double x = start_x; x < finish_x; x+= delta){
		for(double y = start_y; y < finish_y; y += delta){
			small_res =0;
			for(int k = 0; k < m; k++){
				sum_sq = (pow(x - a1[k], 2) + pow(y - a2[k], 2));
				expon = exp(-1.0/M_PI * sum_sq);
				small_res -= c[k] * expon * cos(M_PI*sum_sq);

			}
			result += small_res*delta*delta;
		}
	}
	mu.lock();
	count += result;
	mu.unlock();
}

void read_matrix( int a[], int len, int start, char* ar[]){
    for(int i=0; i < len; i++)
        a[i] = atoi(ar[start + i]);
}


int main(int argc, char* argv[]){
    auto start_time = get_current_time_fenced();

    double start_x = -10.0, finish_x = 10.0, start_y = -10.0, finish_y = 10.0, eps_num_div = 1000;
    int m = 5, threads_amount = 4;
    int c[] = {2,1,4,7,2}, a1[] = {1, 2, 1, 1, 5}, a2[] =  {4, 5, 1, 2, 4};

    if (argc >= 2){

        threads_amount = atoi(argv[1]);
        if(threads_amount <= 0){
            std::cout<<"Number of threads needs to be larger than 0." << std::endl;
            return 1;
        }
        if(argc >= 7){
            start_x = atof(argv[2]);
            finish_x = atof(argv[3]);
            start_y = atof(argv[4]);
            finish_y = atof(argv[5]);
            m = atoi(argv[6]);
            // 3 number of matrix we need for formula (c, a1, a2)
            if(argc >= 6 + m * 3){
                int c[m], a1[m], a2[m];

                // 6 number of arguments before matrix + 1 to read new number
                read_matrix(c, m, 7, argv);
                read_matrix(a1, m, 7 + m, argv);
                read_matrix(a2, m, 7 + 2*m, argv);
                if (argc > 6 + m * 3){
                    eps_num_div = atoi(argv[7 + m * 3]);
                }
            }
            else {
                std::cout<<"Too few parameters." << std::endl;
                return 1;
            }


        }
        else if(argc > 2){
            eps_num_div = atoi(argv[2]);
        }
        if(eps_num_div < 10){
            std::cout<<"Too little partition, try 1000." << std::endl;
            return 1;
        }
    }
    double delta = (finish_x - start_x)*1.0 / eps_num_div;
    double count = 0;

	// vector<thread> t;
	// t.emplace_back(funct, a,b,c);
    std::mutex mux;
    std::vector<std::thread> threads;

    double delta_th = (finish_x - start_x)*1.0 / threads_amount;

    for (int i = 0; i < threads_amount - 1; ++i) {
            threads.emplace_back(worker, std::ref(count), i * delta_th + start_x, (i+1) * delta_th + start_x, start_y, finish_y, m, c, a1, a2, delta, std::ref(mux));
    }
    // last thread must proceed all items to end
    threads.emplace_back(worker, std::ref(count), (threads_amount - 1) * delta_th + start_x, finish_x, start_y, finish_y, m, c, a1, a2, delta, std::ref(mux));

    // join threads
    for (int i = 0; i < threads_amount; ++i) {
        threads[i].join();
    }


    auto finish_time = get_current_time_fenced();
    auto total_time = finish_time - start_time;
    std::cout<<count<<std::endl;
    std::cout << "Total time: " << to_us(total_time) << std::endl;

    return 0;
}
