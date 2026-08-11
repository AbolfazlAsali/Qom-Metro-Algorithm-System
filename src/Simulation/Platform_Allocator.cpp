#include "Simulation/Platform_Allocator.hpp"

#include <algorithm>

using namespace std;

namespace QomMetro::Simulation {

Allocation_Result Platform_Allocator::allocate(const vector<Train>& trains) const{

    Allocation_Result result;

    vector<Train> sorted = trains;
    sort
    (
        sorted.begin(), sorted.end(),
        [](const Train& a, const Train& b) {
            return a.departure_min < b.departure_min;
        }
    );

    double last_departure = -1.0; 

    for(const Train& train : sorted){
      
        if(train.arrival_min >= last_departure){

            result.scheduled.push_back(train);
            last_departure = train.departure_min;
        }
    }

    return result;
}

} // namespace QomMetro::Simulation