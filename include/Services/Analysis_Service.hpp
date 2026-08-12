#pragma once

#include <vector>

#include "Algorithms/Floyd_Warshall.hpp"
#include "Algorithms/IShortest_Path.hpp"
#include "Core/IGraph.hpp"
#include "Core/Weight_Type.hpp"
#include "Simulation/Statistics.hpp"

namespace QomMetro::Services {

// Result of the emergency-team placement heuristic (T4.4, bonus).
struct Emergency_Placement_Result {
    // Stations chosen to host an emergency team, such that every other
    // station is at most one hop from some chosen station.
    std::vector<int> team_station_ids;
};


class Analysis_Service {
public:

    Analysis_Service() = default;

    Algorithms::Path_Result shortest_path(
        const Core::IGraph& graph, int from, int to, Core::Weight_Type weight_type) const;

    Simulation::Statistics_Result analyze_operations(
        const std::vector<Simulation::Visit_Record>& visits) const;

    Emergency_Placement_Result place_emergency_teams(const Core::IGraph& graph) const;

private:

    mutable Algorithms::Floyd_Warshall floyd_warshall_;
    mutable bool floyd_warshall_built_ = false;
    mutable Core::Weight_Type built_weight_type_ = Core::Weight_Type::Distance;

    Simulation::Statistics statistics_algorithm_;
};

} // namespace QomMetro::Services