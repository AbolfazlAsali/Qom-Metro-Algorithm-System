#pragma once

#include <vector>

#include "Core/IGraph.hpp"
#include "Core/Weight_Type.hpp"

namespace QomMetro::Algorithms {

// A single edge in an MST result.
struct Mst_Edge {

    int from;
    int to;
    double weight;
};

// Result of building a minimum spanning tree.
struct Mst_Result{

    // False if the graph is disconnected: no single tree can span every
    // station. When false, `edges` still holds the minimum spanning.
    bool success = false;

    std::vector<Mst_Edge> edges;
    double total_weight = 0.0;
};

// Builds a minimum spanning tree over the entire graph under a chosen
// weight criterion (distance = construction cost, per the project spec).
class IMst{

public:

    virtual ~IMst() = default;

    virtual Mst_Result run(

        const Core::IGraph& graph, 
        Core::Weight_Type weight_type
    
    ) const = 0;
};

} // namespace QomMetro::Algorithms