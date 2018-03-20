#include <cmath>
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <vector>
//#include "time.cpp" // TODO time.h

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}

double f(double x, double y)
{

    double res1 = 0, res2 = 0;

    for (int i = 1; i <= 5; ++i)
    {
        res1 += i * cos((i + 1) * x + 1);
        res2 += i * cos((i + 1) * y + 1);
    }
    return -res1 * res2;
}

std::mutex mu;

void integral(int n, int st, int fn, const int bounds[], double &res)
{

    int lboundX = bounds[0], uboundX = bounds[1], lboundY = bounds[2], uboundY = bounds[3];

    double dy = (double) (uboundY - lboundY) / n;
    double dx = (double) (uboundX - lboundX) / n;

    double sum = 0;

    for (int i = 0; i < n; ++i)
    {

        double y_val = lboundY + dy * i;
        double innerSum = 0;

        for (int j = st; j < fn; ++j)
        {

            double x_val = lboundX + dx * j;
            double funcRes = f(x_val, y_val);

            innerSum += funcRes * dx;

        }

        sum += innerSum * dy;

    }

    mu.lock();
    res += sum;
    mu.unlock();

}

// -5.045849736

void handleArgs(int argc, char *argv[], double &abs_err, double &rel_err, int &numOfThreads, int &n, int *bounds)
{
    /*  - Бажану абсолютну точність (пояснення цих термінів -- на парах, крім того, див. нижче)
        - Бажану відносну точність (не менше 0.001 або 0.1%)
        - Кількість потоків, яку слід використати
        - n
        - Інтервал інтегрування по x та y
    */

    if (argc != 9)
    {
        std::cout << "Wrong # of arguments\n";
        exit(-1);
    }

    abs_err = atof(argv[1]);
    rel_err = atof(argv[2]);
    numOfThreads = atoi(argv[3]);
    n = atoi(argv[4]);
    bounds[0] = atoi(argv[5]), bounds[1] = atoi(argv[6]), bounds[2] = atoi(argv[7]), bounds[3] = atoi(argv[8]);
}


int main(int argc, char *argv[])
{
    double abs_err, rel_err;
    int numOfThreads;
    int n;
    int bounds[] = {-100, 100, -100, 100};

    handleArgs(argc, argv, abs_err, rel_err, numOfThreads, n, bounds);

    std::vector<std::thread> threads;

    double res = 0;

    int x = n / numOfThreads, y = 0;

    auto start = get_current_time_fenced();

    for (int i = 0; i < numOfThreads; i++)
    {
        y += x;
        int st = y - x;
        int fn = y;
        threads.emplace_back(integral, n, st, fn, bounds, std::ref(res));
    }

    for (auto &thread : threads) thread.join();

    float finish = to_us(get_current_time_fenced() - start);

    std::cout << "Total time: " << finish / 1000000 << std::endl;
    std::cout << "Result: " << res << std::endl;

}
