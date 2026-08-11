#pragma once

#include <vector>

namespace QomMetro::Simulation {

// One train's requested use of the shared platform.
struct Train{

    int id;
    double arrival_min;   // arrival time, in minutes from some reference point
    double departure_min; // departure time, in minutes from the same reference point
};

// Result of allocating trains to the shared platform.
struct Allocation_Result{
    // Trains selected for the platform, in departure-time order.
    std::vector<Train> scheduled;
};


class Platform_Allocator {
public:

    Allocation_Result allocate(const std::vector<Train>& trains) const;
};

} // namespace QomMetro::Simulation