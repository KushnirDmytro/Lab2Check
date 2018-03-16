#include <iostream>
#include <mutex>


#include "../settings/settings.h"


namespace integral{
    extern const double
        INITIAL_DELTA,
        STEP_DELTA_MULTIPLIER,
        EPS;
    extern const int
        OUTPUT_PRECISION;


    class Point{
    public:
        double x, y;
        Point(double x, double y);
    };


    double calc_s(Point &p1, Point &p2);


    class Estimator{
    public:
        Estimator(settings :: SettingsHolder settings);

        double get_relative_error(double result);
        double get_absolute_error(double result);
    private:
        double STANDARD;
    };


    class Calculator{
    public:
        Calculator(settings :: SettingsHolder settings);
        double evaluate(Point &p);
    private:
        double A, B, C;
    };


    class Result{
    public:
        double result;
        double absolute_error, relative_error;

        Result(double result, double absolute_error, double relative_error);
    };


    std::ostream& operator<<(std::ostream &strm, Result &target);


    class Integrator{
    public:
        Integrator(settings :: SettingsHolder settings);
        ~Integrator();

        Result run();

    private:
        int iterations_left;
        double DELTA;

        settings :: SettingsHolder settings;

        double integrate();

        void integrate_on_rectangular(
            Point upper_left,
            Point lower_right,
            std::mutex &mux,
            double &target
        );

        Calculator* calculator;
        Estimator* estimator;
    };
}