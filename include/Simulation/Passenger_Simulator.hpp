#pragma once

#include <vector>

namespace QomMetro::Simulation {

// Parameters controlling one simulation run.
struct Simulation_Config{

    double duration_min;        // total simulated time span, in minutes
    double arrival_rate_per_min;  // average passengers arriving per minute
    double mean_service_time_min; // average time to process one passenger at a gate
    int gate_count;             // number of parallel gates/turnstiles
    unsigned int random_seed;   // fixes the random sequence, for reproducible runs
};

// Aggregated results of one simulation run.
struct Simulation_Result{

    int total_passengers = 0;
    double average_wait_min = 0.0;
    double max_wait_min = 0.0;
};


class Passenger_Simulator{
public:

    Simulation_Result simulate(const Simulation_Config& config) const;
};

} // namespace QomMetro::Simulation