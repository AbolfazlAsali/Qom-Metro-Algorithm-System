#include "Services/Routing_Service.hpp"

namespace QomMetro::Services {

Routing_Service::Routing_Service(
    
    const Algorithms::IReachability& reachability_algorithm,
    const Algorithms::IShortest_Path& shortest_path_algorithm
)
    : reachability_algorithm_(reachability_algorithm),
      shortest_path_algorithm_(shortest_path_algorithm) {
}

Algorithms::Reachability_Result Routing_Service::check_reachability(
    const Core::IGraph& graph, int from, int to) const {
    return reachability_algorithm_.run(graph, from, to);
}

Algorithms::Path_Result Routing_Service::find_shortest_path(
    const Core::IGraph& graph, int from, int to, Core::Weight_Type weight_type) const{
    return shortest_path_algorithm_.run(graph, from, to, weight_type);
}

} // namespace QomMetro::Services