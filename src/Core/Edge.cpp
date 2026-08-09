#include "Core/Edge.hpp"

#include <cassert>

namespace QomMetro::Core{

Edge::Edge(int to, double distance_km, double time_min)
    : to_(to), distance_km_(distance_km), time_min_(time_min){
}

int Edge::to() const{
    return to_;
}

double Edge::distance_km() const{
    return distance_km_;
}

double Edge::time_min() const{
    return time_min_;
}

double Edge::weight(Weight_Type type) const{

    switch(type){

        case Weight_Type::Distance: return distance_km_;
        case Weight_Type::Time:     return time_min_;
    }
    assert(false && "Unhandled WeightType in Edge::weight");
    return distance_km_;
}

double Edge::bonus_weight() const{
    return bonus_weight_;
}

void Edge::set_bonus_weight(double bonus){
    bonus_weight_ = bonus;
}

double Edge::effective_weight(Weight_Type type) const{
    return weight(type) + bonus_weight_;
}

double Edge::capacity() const{
    return capacity_;
}

void Edge::set_capacity(double capacity){
    capacity_ = capacity;
}

} // namespace QomMetro::Core