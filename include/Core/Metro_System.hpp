#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Algorithms/BFS.hpp"
#include "Algorithms/Dijkstra.hpp"
#include "Algorithms/Edmonds_Karp.hpp"
#include "Algorithms/Kruskal.hpp"
#include "Core/Graph.hpp"
#include "IO/File_Loader.hpp"
#include "Services/Analysis_Service.hpp"
#include "Services/Network_Service.hpp"
#include "Services/Routing_Service.hpp"
#include "Services/Search_Service.hpp"
#include "Simulation/Statistics.hpp"

namespace QomMetro::Core {


class Metro_System {
public:

    // Builds the graph via `loader`, then wires every Service to this
    // system's default algorithm choices.
    Metro_System(
        const IO::File_Loader& loader,
        const std::string& stations_file_path,
        const std::string& edges_file_path,
        const std::string& capacities_file_path = "",
        const std::string& incentives_file_path = ""
    );

   
    Algorithms::Reachability_Result check_reachability(int from, int to) const;
    Algorithms::Path_Result find_shortest_path(
        int from, int to, Weight_Type weight_type) const;

   
    Algorithms::Mst_Result build_minimum_spanning_tree(Weight_Type weight_type) const;
    Algorithms::Max_Flow_Result compute_max_flow(int source, int sink) const;
    Algorithms::Articulation_Result find_critical_stations() const;


    Algorithms::Path_Result shortest_path_precomputed(
        int from, int to, Weight_Type weight_type) const;
    Simulation::Statistics_Result analyze_operations(
        const std::vector<Simulation::Visit_Record>& visits) const;
    Services::Emergency_Placement_Result place_emergency_teams() const;

    
    std::vector<Algorithms::Search_Match> find_station(const std::string& query) const;

    // Direct read access to the underlying graph, for callers (like the
    // CLI) that need to list stations, resolve names to ids, etc.
    const IGraph& graph() const;

private:

    std::unique_ptr<Graph> graph_;

    Algorithms::Bfs_Reachability default_reachability_;
    Algorithms::Dijkstra default_shortest_path_;
    Algorithms::Kruskal default_mst_;
    Algorithms::Edmonds_Karp default_max_flow_;

    Services::Routing_Service routing_service_;
    Services::Network_Service network_service_;
    Services::Analysis_Service analysis_service_;
    Services::Search_Service search_service_;
};

} // namespace QomMetro::Core