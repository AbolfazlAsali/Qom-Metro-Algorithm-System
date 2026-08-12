#pragma once

#include "Algorithms/IReachability.hpp"
#include "Algorithms/IShortest_Path.hpp"
#include "Core/IGraph.hpp"

namespace QomMetro::Services {

class Routing_Service {
public:

    Routing_Service
    (
        const Algorithms::IReachability& reachability_algorithm,
        const Algorithms::IShortest_Path& shortest_path_algorithm
    );

    Algorithms::Reachability_Result check_reachability
    (
        const Core::IGraph& graph, 
        int from, 
        int to
    ) const;

    Algorithms::Path_Result find_shortest_path
    (
        const Core::IGraph& graph,
        int from,
        int to,
        Core::Weight_Type weight_type
    ) const;

private:
    const Algorithms::IReachability& reachability_algorithm_;
    const Algorithms::IShortest_Path& shortest_path_algorithm_;
};

} // namespace QomMetro::Services