#include <iostream>
#include <boost/program_options.hpp>
#include <chrono>
#include <atomic>

namespace std {
    std::ostream &operator<<(std::ostream &os, const std::pair<double, double> &pair) {
        os << pair.first << " " << pair.second;

        return os;
    }

    std::istream &operator>>(std::istream &is, std::pair<double, double> &pair) {
        string s;
        is >> s;
        const size_t sep = s.find(':');
        if (sep==string::npos) {
            cerr << "You have to write range in the following format: number:number (-40:3)\n";
            exit(2);
        } else {
            pair.first = atof(s.substr(0,sep).c_str());
            pair.second = atof(s.substr(sep+1).c_str());
        }

        return is;
    }
}

using namespace std;

inline chrono::high_resolution_clock::time_point get_current_time_fenced() {
    atomic_thread_fence(memory_order_seq_cst);
    auto res_time = chrono::high_resolution_clock::now();
    atomic_thread_fence(memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return chrono::duration_cast<chrono::microseconds>(d).count();
}

double fn(double x1, double x2) {
    double res = 0.002;

    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            res += (1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6)));
        }
    }

    res = 1 / res;

    return res;
}


template<typename func_T>
double integrate(func_T fn, double min_x, double max_x, double min_y, double max_y, size_t steps) {
    double res = 0;

    double delta_x = (max_x - min_x) / steps;
    double delta_y = (max_y - min_y) / steps;

    double x1 = min_x;
    double x2 = max_x;
    double y1 = min_y;
    double y2 = max_y;

    while (x1 < x2) {
        while (y1 < y2) {
            double xi_ = (x1 + (x1 + delta_x)) / 2;
            double yi_ = (y1 + (y1 + delta_y)) / 2;

            res += fn(xi_, yi_) * delta_x * delta_y;

            y1 += delta_y;
        }

        x1 += delta_x;
        y1 = min_y;
    }

    return res;
}

int main(int argc, char **argv) {
    ///////////////////////////////////////////////////////////////////////////////////
    // Start program options workflow                                   //////////////
    ///////////////////////////////////////////////////////////////////////////////////

    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "Help me! or 'Houston, we have a problem!'")
            ("version,v", "Version of the program")
            ("absolute,a", po::value<double>()->default_value(0.1),
             "Desired absoluteError precision for calculations. Default: 0.1")
            ("relative,r", po::value<double>()->default_value(0.05),
             "Desired relativeError precision for calculations in %. No less than 0.1. Default: 0.05")
            ("threads,t", po::value<int>()->default_value(2), "Numbers of threads involved in computing. Default: 2")
            ("xrange,x", po::value<std::pair<double, double>>()->default_value(make_pair(-50.0, 50.0)),
             "Range of values for x.\nDefault: -50...50")
            ("yrange,y", po::value<pair<double, double>>()->default_value(make_pair(-50.0, 50.0)),
             "Range of values for y.\nDefault: -50...50");

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
    po::notify(vm);

    // Validate values
    pair<double, double> xrange = vm["xrange"].as<pair<double, double>>();
    pair<double, double> yrange = vm["yrange"].as<pair<double, double>>();
    double absoluteError = vm["absolute"].as<double>();
    double relativeError = vm["relative"].as<double>();
    int threads = vm["threads"].as<int>();

    if(xrange.first > xrange.second){
        printf("Incorrect value for xrange!\n");
        exit(2);
    }
    if(yrange.first > yrange.second){
        printf("Incorrect value for yrange!\n");
        exit(2);
    }
    if(absoluteError <= 0){
        printf("Incorrect value for absoluteError error. It should be greater than 0!\n");
        exit(2);
    }
    if(relativeError <= 0){
        printf("Incorrect value for relativeError error. It should be greater than 0!\n");
        exit(2);
    }
    if(threads <= 0){
        printf("Incorrect value for number of threads. It should be greater than 0!\n");
        exit(2);
    }

    // Print help and version of the program
    if (vm.count("help")) {
        cout << desc << "\n";
        return 0;
    }

    if (vm.count("version")) {
        cout << "Version: 1.0" << "\n";
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    // Start integration                                                 //////////////
    ///////////////////////////////////////////////////////////////////////////////////
    auto start_time = get_current_time_fenced();

    size_t steps = 250;
    double res1, res2, concreteAbsError, concreteRelError;

    res1 = integrate(fn, xrange.first, xrange.second, yrange.first, yrange.second, steps);

    while(true){
        steps *= 2;
        res2 = integrate(fn, xrange.first, xrange.second, yrange.first, yrange.second, steps);

        // check absoluteError and relativeError error
        concreteAbsError = fabs(res2 - res1);
        concreteRelError = fabs(1 - (res1 / res2));

        if((concreteAbsError < absoluteError) && (concreteRelError < relativeError) ){
            break;
        } else {
            res1 = res2;
        }
    }

    auto finish_time = get_current_time_fenced();

    printf("Computing time: %lld\n", to_us(finish_time - start_time));
    printf("Absolute error: %lf\n", concreteAbsError);
    printf("Relative error: %.15lf\n", concreteRelError);
    printf("Result: %f\n", res2);

    return 0;
}