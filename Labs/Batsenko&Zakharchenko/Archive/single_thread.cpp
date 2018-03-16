#include <iostream>

#include <math.h>

#include <chrono>
#include <atomic>

# define M_PI 3.14159265358979323846

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


double worker(double start_x, double finish_x, double start_y, double finish_y,  int m, int c[], int a1[], int a2[], double delta){
	double result = 0;
	double small_res =0;
	for(double x = start_x; x < finish_x; x+= delta){
		for(double y = start_y; y < finish_y; y += delta){
			small_res =0;
			for(int k = 0; k < m; k++)
				small_res -= function(c[k], x, y, a1[k], a2[k]);
			result += small_res*delta*delta;
		}
	}
	return result;
}
void read_matrix( int a[], int len, int start, char* ar[]){
    for(int i=0; i < len; i++)
        a[i] = atoi(ar[start + i]);
}


int main(int argc, char* argv[]){
	auto start_time = get_current_time_fenced();
	double start_x = -10.0, finish_x = 10.0, start_y = -10.0, finish_y = 10.0, eps_num_div = 1000 ;
	int m = 5, c[] = {2,1,4,7,2}, a1[] = {1, 2, 1, 1, 5}, a2[] =  {4, 5, 1, 2, 4};
	if (argc >= 2){


        if(argc >= 6){
            start_x = atof(argv[1]);
            finish_x = atof(argv[2]);
            start_y = atof(argv[3]);
            finish_y = atof(argv[4]);
            m = atoi(argv[5]);
            // 3 number of matrix we need for formula (c, a1, a2)
            if(argc >= 5 + m * 3){
                int c[m], a1[m], a2[m];

                // 5 number of arguments before matrix + 1 to read new number
                read_matrix(c, m, 6, argv);
                read_matrix(a1, m, 6 + m, argv);
                read_matrix(a2, m, 6 + 2*m, argv);
                if (argc > 5 + m * 3){
                    eps_num_div = atoi(argv[6 + m * 3]);

                }
            }
            else {
                std::cout<<"Too few parameters." << std::endl;
                return 1;
            }


        }
        else if(argc > 1){
            eps_num_div = atoi(argv[1]);
        }
        if(eps_num_div < 10){
            std::cout<<"Too little partition, try 1000." << std::endl;
            return 1;
	}
        }
	double delta = (finish_x - start_x)*1.0 / eps_num_div;
	double res = worker(start_x, finish_x, start_y, finish_y, m, c, a1, a2, delta);

	auto finish_time = get_current_time_fenced();
	auto total_time = finish_time - start_time;

	std::cout<<res<<std::endl;
	std::cout << "Total time: " << to_us(total_time) << std::endl;
	return 0;
}
