#include "Services/Network_Service.hpp"

namespace QomMetro::Services {

Network_Service::Network_Service(
    const Algorithms::IMst& mst_algorithm,
    const Algorithms::IMax_Flow& max_flow_algorithm)
    : mst_algorithm_(mst_algorithm), max_flow_algorithm_(max_flow_algorithm){
}

Algorithms::Mst_Result Network_Service::build_minimum_spanning_tree(
    const Core::IGraph& graph, Core::Weight_Type weight_type) const {
    return mst_algorithm_.run(graph, weight_type);
}

Algorithms::Max_Flow_Result Network_Service::compute_max_flow(
    const Core::IGraph& graph, int source, int sink) const {
    return max_flow_algorithm_.run(graph, source, sink);
}

Algorithms::Articulation_Result Network_Service::find_critical_stations(
    const Core::IGraph& graph) const {
    return articulation_points_.run(graph);
}

} // namespace QomMetro::Services