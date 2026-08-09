#pragma once

#include <vector>

#include "Core/IGraph.hpp"

namespace QomMetro::Algorithms {

// Result of a reachability query between two stations.
struct Reachability_Result{

    bool reachable = false;

    // A valid path from `from` to `to` (station ids, in order), if
    // reachable. Empty if not reachable.
    std::vector<int> path;
};

// Determines whether a path exists between two stations, and if so,
// returns one valid path.
class IReachability{

public:

    virtual ~IReachability() = default;

    virtual Reachability_Result run(

        const Core::IGraph& graph, 
        int from, 
        int to
    
    ) const = 0;
};

} // namespace QomMetro::Algorithms