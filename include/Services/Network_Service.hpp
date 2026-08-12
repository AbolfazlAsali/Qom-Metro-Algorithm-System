#pragma once

#include "Algorithms/Articulation_Points.hpp"
#include "Algorithms/IMax_Flow.hpp"
#include "Algorithms/IMst.hpp"
#include "Core/IGraph.hpp"

namespace QomMetro::Services {

class Network_Service {
public:

    Network_Service(
        const Algorithms::IMst& mst_algorithm,
        const Algorithms::IMax_Flow& max_flow_algorithm);

    Algorithms::Mst_Result build_minimum_spanning_tree(
        const Core::IGraph& graph, Core::Weight_Type weight_type) const;

    Algorithms::Max_Flow_Result compute_max_flow(
        const Core::IGraph& graph, int source, int sink) const;

    Algorithms::Articulation_Result find_critical_stations(
        const Core::IGraph& graph) const;

private:
    const Algorithms::IMst& mst_algorithm_;
    const Algorithms::IMax_Flow& max_flow_algorithm_;
    Algorithms::Articulation_Points articulation_points_;
};

} // namespace QomMetro::Services