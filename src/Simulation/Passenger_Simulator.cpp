#include "Simulation/Passenger_Simulator.hpp"

#include <algorithm>
#include <random>

#include "Structures/Min_Heap.hpp"

using namespace std;

namespace QomMetro::Simulation {

Simulation_Result Passenger_Simulator::simulate(const Simulation_Config& config) const{


    Simulation_Result result;

    if(config.gate_count <= 0 || config.duration_min <= 0.0){
        return result; 
    }

    mt19937 rng(config.random_seed);
    exponential_distribution<double> inter_arrival(config.arrival_rate_per_min);
    exponential_distribution<double> service_time(1.0 / config.mean_service_time_min);

   
    Structures::Min_Heap gate_freeAt;
    for(int gate = 0; gate < config.gate_count; ++gate){
        gate_freeAt.push(0.0, gate);
    }

    double current_time = 0.0;
    double total_wait = 0.0;
    double max_wait = 0.0;

    while(true){

        current_time += inter_arrival(rng);
        if(current_time > config.duration_min){
            break; 
        }

      
        Structures::Heap_Item next_free_gate = gate_freeAt.extract_min();
        double gate_free_time = next_free_gate.priority;
        int gate_id = next_free_gate.id;

        
        double wait = max(0.0, gate_free_time - current_time);

        double service_duration = service_time(rng);
        double new_free_time = current_time + wait + service_duration;
        gate_freeAt.push(new_free_time, gate_id);

        total_wait += wait;
        max_wait = max(max_wait, wait);
        result.total_passengers++;
    }

    result.average_wait_min = (result.total_passengers > 0)
        ? total_wait / static_cast<double>(result.total_passengers)
        : 0.0;


    result.max_wait_min = max_wait;
    return result;
}

} // namespace QomMetro::Simulation