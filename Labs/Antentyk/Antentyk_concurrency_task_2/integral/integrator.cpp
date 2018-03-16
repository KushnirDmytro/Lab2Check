#include <vector>
#include <mutex>
#include <thread>
#include <algorithm>
#include <functional>
#include <cstdlib>


#include "integral.h"


using namespace integral;


Integrator :: Integrator (settings :: SettingsHolder settings){
    this->settings = settings;

    calculator = new Calculator(settings);
    estimator = new Estimator(settings);

    iterations_left = settings.ITERATIONS_NUM;
    DELTA = INITIAL_DELTA;
}


Integrator :: ~Integrator(){
    delete(calculator);
    delete(estimator);
}


void Integrator :: integrate_on_rectangular(
    Point upper_left,
    Point lower_right,
    std::mutex &mux,
    double &target
)
{
    auto safe_change_x = [upper_left, lower_right](
        double &target,
        double value
    ){
        target += value;
        target = std::min(target, lower_right.x);
        target = std::max(target, upper_left.x);
    };

    auto safe_change_y = [upper_left, lower_right](
        double &target,
        double value
    ){
        target += value;
        target = std::max(target, lower_right.y);
        target = std::min(target, upper_left.y);
    };


    double result = 0;


    Point cur_upper_left{upper_left.x, upper_left.y};
    Point cur_lower_right{upper_left.x, upper_left.y};


    safe_change_x(cur_lower_right.x, DELTA);
    safe_change_y(cur_lower_right.y, -DELTA);


    while(cur_upper_left.y > cur_lower_right.y){
        cur_upper_left.x = upper_left.x;

        cur_lower_right.x = upper_left.x;
        safe_change_x(cur_lower_right.x, DELTA);
        
        while(cur_upper_left.x < cur_lower_right.x){
            double current = calc_s(cur_upper_left, cur_lower_right);
            current *= calculator->evaluate(cur_upper_left);

            result += current;

            cur_upper_left.x += DELTA;
            safe_change_x(cur_lower_right.x, DELTA);
        }

        cur_upper_left.y -= DELTA;
        safe_change_y(cur_lower_right.y, -DELTA);
    }

    std::lock_guard<std::mutex> lck(mux);
    target += result;
}


double Integrator :: integrate(){
    double answer = 0;
    std::mutex mux;

    if(settings.IS_CONSECUTIVE){
        Point upper_left{settings.MIN_X, settings.MAX_Y};
        Point lower_right{settings.MAX_X, settings.MIN_Y};
        integrate_on_rectangular(
            upper_left,
            lower_right,
            mux,
            answer
        );
    }
    else{
        double h_rectangular = (settings.MAX_Y - settings.MIN_Y) / settings.THREADS_NUM;

        Point upper_left{settings.MIN_X, settings.MAX_Y};
        Point lower_right{settings.MAX_X, settings.MAX_Y - h_rectangular};

        std::vector<std::thread> threads;       

        for(
            int i = 0;
            i < settings.THREADS_NUM;
            i++, upper_left.y -= h_rectangular, lower_right.y -= h_rectangular
        )
        {
            threads.push_back(
                std::thread(
                    &integral::Integrator::integrate_on_rectangular,
                    this,
                    upper_left,
                    lower_right,
                    std::ref(mux),
                    std::ref(answer)
                )
            );
        }

        for(int i = 0; i < settings.THREADS_NUM; i++)
            threads[i].join();
    }

    return answer;
}


Result Integrator :: run(){
    double current_result = 0;

    for(;iterations_left > 0; iterations_left--, DELTA *= STEP_DELTA_MULTIPLIER){
        current_result = integrate();

        if(estimator->get_absolute_error(current_result) > 
            settings.ABSOLUTE_ERROR)
            continue;
        if(estimator->get_relative_error(current_result) > 
            settings.RELATIVE_ERROR)
            continue;
        
        return Result(
            current_result,
            estimator->get_absolute_error(current_result),
            estimator->get_relative_error(current_result)
        );
    }

    return Result(
        current_result,
        estimator->get_absolute_error(current_result),
        estimator->get_relative_error(current_result)
    );
}