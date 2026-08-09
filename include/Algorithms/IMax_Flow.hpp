#pragma once

#include "Core/IGraph.hpp"

namespace QomMetro::Algorithms {

// Result of a max-flow computation between a source and a sink.
struct Max_Flow_Result{
    // Maximum number of passengers per unit time that can move from
    // source to sink, respecting every edge's capacity (Core::Edge::capacity()).
    double max_flow = 0.0;
};

// Computes the maximum flow between two stations, using edge capacities.
class IMax_Flow{

public:

    virtual ~IMax_Flow() = default;

    virtual Max_Flow_Result run(

        const Core::IGraph& graph, 
        int source, 
        int sink
    
    ) const = 0;
};

} // namespace QomMetro::Algorithms