#include "Simulation/Dispatch_Queue_Manager.hpp"

using namespace std;

namespace QomMetro::Simulation {

void Dispatch_Queue_Manager::add_or_update(int trainId, double priority){
    queue_.push_or_decrease(trainId, priority);
}

optional<int> Dispatch_Queue_Manager::dispatch_next(){

    if(queue_.empty()){
        return nullopt;
    }
    return queue_.pop_min();
}

bool Dispatch_Queue_Manager::empty(){
    return queue_.empty();
}

} // namespace QomMetro::Simulation