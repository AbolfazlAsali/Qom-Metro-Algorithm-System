#pragma once

#include "Core/Weight_Type.hpp"
#include "Utils/Constants.hpp"

namespace QomMetro::Core {


class Edge {
public:

    Edge(int to, double distance_km, double time_min);

    int to() const;
    double distance_km() const;
    double time_min() const;


    double weight(Weight_Type type) const;

    double bonus_weight() const;
    void set_bonus_weight(double bonus);

  
    double effective_weight(Weight_Type type) const;

  
    double capacity() const;
    void set_capacity(double capacity);

private:

    int to_;
    double distance_km_;
    double time_min_;
    double bonus_weight_ = 0.0;
    double capacity_ = Utils::kInfinity;
};

} // namespace QomMetro::Core