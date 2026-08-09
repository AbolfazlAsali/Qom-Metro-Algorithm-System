#pragma once

#include <vector>

#include "Core/IGraph.hpp"
#include "Core/Weight_Type.hpp"

namespace QomMetro::Algorithms {

// Result of a single-pair shortest-path query.
struct Path_Result {

    bool reachable = false;

    // Station ids from `from` to `to`, in order. Empty if not reachable.
    std::vector<int> path;

    // Total cost of `path` under the chosen WeightType. Only meaningful
    // when reachable is true.
    double total_cost = 0.0;

    bool has_negative_cycle = false;

    // Number of stations "expanded" (finalized/popped from the frontier)
    // during the search. 
    int nodes_expanded = 0;
};

// Computes the shortest path between two stations under a chosen weight
// criterion.
class IShortest_Path {
public:
    virtual ~IShortest_Path() = default;

    virtual Path_Result run(

        const Core::IGraph& graph,
        int from,
        int to,
        Core::Weight_Type weight_type
    
    ) const = 0;
};

} // namespace QomMetro::Algorithms