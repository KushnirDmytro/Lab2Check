#include <iostream>
#include <fstream>
#include <chrono>
#include <atomic>
#include <thread>
#include <vector>
#include <cmath>

#include <boost/program_options.hpp>

auto get_time_fenced()
{
    std::atomic_thread_fence(std::memory_order_seq_cst);

    auto time = std::chrono::high_resolution_clock::now();

    std::atomic_thread_fence(std::memory_order_seq_cst);

    return time;
}

auto get_options(std::string config_filename) 
{
    namespace po = boost::program_options;

    po::options_description description("");

    description.add_options()
        ("threads", po::value<int>()->required())
        ("err_abs", po::value<double>()->required()) 
        ("err_rel", po::value<double>()->required())
        ("x_min",   po::value<double>()->required()) 
        ("x_max",   po::value<double>()->required())
        ("y_min",   po::value<double>()->required()) 
        ("y_max",   po::value<double>()->required());

    po::variables_map vm;

    try 
    {
        std::ifstream config_file(config_filename);

        if (!config_file.is_open()) 
        {         
            std::cerr << "Configuration file (" << config_filename << ") is missing" << std::endl;
            exit(1);
        }

        store(po::parse_config_file(config_file, description), vm);
        po::notify(vm); 
    } 
    catch (std::exception& e)
    {
        std::cerr << "Error in configuration file: " << e.what() << std::endl;
        exit(1);
    } 

    return vm;
}

double ackley(double x, double y, double a = 20.0, double b = 0.2, double c = 2.0 * std::acos(-1.0))
{
    double term_1 = -a * std::exp(-b * std::sqrt(0.5 * (x * x + y * y)));
    
    double term_2 = std::exp(0.5 * (std::cos(c * x) + std::cos(c * y)));

    return term_1 - term_2 + a + std::exp(1.0);
}

void integrate_thread(
    double &result, 
    double x_start, double x_end, 
    double y_min,   double y_max, 
    double x_step,  double y_step)
{  
    double area = 0;

    for (double x = x_start; x < x_end - 1e-8; x += x_step) 
    {
        for (double y = y_min; y < y_max - 1e-8; y += y_step) 
        {
            area += ackley(x, y) * x_step * y_step;  
        }
    }
    
    result = area;
}

double integrate(boost::program_options::variables_map vm, double delta) 
{
    std::vector<double> results(vm["threads"].as<int>());

    std::vector<std::thread> threads;

    double x_interval = std::abs(vm["x_max"].as<double>() - vm["x_min"].as<double>());
    double y_interval = std::abs(vm["y_max"].as<double>() - vm["y_min"].as<double>());

    double thread_interval = x_interval / vm["threads"].as<int>();

    double x_start = vm["x_min"].as<double>();

    for (int i = 0; i < vm["threads"].as<int>(); ++i)
    {
        threads.push_back(std::thread(integrate_thread, 
            std::ref(results[i]), 
            x_start, x_start + thread_interval, 
            vm["y_min"].as<double>(), vm["y_max"].as<double>(),
            x_interval / delta, y_interval / delta
            ));

        x_start = x_start + thread_interval;
    }
  
    for (auto& thread : threads) thread.join(); 

    return std::accumulate(results.begin(), results.end(), 0.0);
}

int main()
{	
    auto vm = get_options("ackley.cfg");

    auto start = get_time_fenced();
    
    const double delta_start = 840.0;

    double old_result = integrate(vm, delta_start);

    double err_abs = 0;
    double err_rel = 0;

    bool is_correct = false;

    for (double delta = delta_start * 2.0; !is_correct; delta *= 2.0) 
    {
        double new_result = integrate(vm, delta);

        err_abs = std::abs(new_result - old_result);
        err_rel = err_abs / new_result;

        is_correct = err_abs < vm["err_abs"].as<double>() && err_rel < vm["err_rel"].as<double>();

        old_result = new_result;
    }

    auto end = get_time_fenced();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout.precision(10); 

    std::cout << std::scientific << old_result << std::endl;
    std::cout << err_abs << " " << err_rel << std::endl;
    std::cout << elapsed << std::endl;

    return 0;
}
