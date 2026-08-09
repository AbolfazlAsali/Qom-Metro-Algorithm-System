#include "Utils/Timer.hpp"
using namespace std;

namespace QomMetro::Utils {

void Timer::start(){

    start_time_ = chrono::steady_clock::now();
    running_ = true;
}

void Timer::stop(){

    end_time_ = chrono::steady_clock::now();
    running_ = false;
}

double Timer::elapsed_milli_seconds() const{

    if(start_time_ == chrono::steady_clock::time_point{}){
        return 0.0;
    }
    
    auto end = running_ ? chrono::steady_clock::now() : end_time_;
    chrono::duration<double, milli> diff = end - start_time_;
    return diff.count();
}

} // namespace QomMetro::Utils