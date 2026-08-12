#include "Core/Metro_System.hpp"

using namespace std;

namespace QomMetro::Core {

Metro_System::Metro_System(
    
    const IO::File_Loader& loader,
    const string& stations_file_path,
    const string& edges_file_path)
    : graph_(loader.load_Graph(stations_file_path, edges_file_path)),
      routing_service_(default_reachability_, default_shortest_path_),
      network_service_(default_mst_, default_max_flow_) {

}

Algorithms::Reachability_Result Metro_System::check_reachability(int from, int to) const {
    return routing_service_.check_reachability(*graph_, from, to);
}

Algorithms::Path_Result Metro_System::find_shortest_path(
    int from, int to, Weight_Type weight_type) const {
    return routing_service_.find_shortest_path(*graph_, from, to, weight_type);
}

Algorithms::Mst_Result Metro_System::build_minimum_spanning_tree(Weight_Type weight_type) const {
    return network_service_.build_minimum_spanning_tree(*graph_, weight_type);
}

Algorithms::Max_Flow_Result Metro_System::compute_max_flow(int source, int sink) const {
    return network_service_.compute_max_flow(*graph_, source, sink);
}

Algorithms::Articulation_Result Metro_System::find_critical_stations() const {
    return network_service_.find_critical_stations(*graph_);
}

Algorithms::Path_Result Metro_System::shortest_path_precomputed(
    int from, int to, Weight_Type weight_type) const {
    return analysis_service_.shortest_path(*graph_, from, to, weight_type);
}

Simulation::Statistics_Result Metro_System::analyze_operations(
    const vector<Simulation::Visit_Record>& visits) const {
    return analysis_service_.analyze_operations(visits);
}

Services::Emergency_Placement_Result Metro_System::place_emergency_teams() const {
    return analysis_service_.place_emergency_teams(*graph_);
}

vector<Algorithms::Search_Match> Metro_System::find_station(const string& query) const {
    return search_service_.find_station(*graph_, query);
}

const IGraph& Metro_System::graph() const {
    return *graph_;
}

} // namespace QomMetro::Core