#include "Services/Analysis_Service.hpp"

#include <unordered_set>

using namespace std;

namespace QomMetro::Services {

Algorithms::Path_Result Analysis_Service::shortest_path(
    const Core::IGraph& graph, 
    int from, 
    int to, 
    Core::Weight_Type weight_type
) const {



    if(!floyd_warshall_built_ || built_weight_type_ != weight_type){

        floyd_warshall_.build(graph, weight_type);
        floyd_warshall_built_ = true;
        built_weight_type_ = weight_type;
    }

    return floyd_warshall_.query(from, to);
}

Simulation::Statistics_Result Analysis_Service::analyze_operations(
    const vector<Simulation::Visit_Record>& visits) const {
    return statistics_algorithm_.analyze(visits);
}


Emergency_Placement_Result Analysis_Service::place_emergency_teams(
    const Core::IGraph& graph) const{

    Emergency_Placement_Result result;

    const vector<Core::Station>& stations = graph.all_stations();
    unordered_set<int> uncovered;
    for(const Core::Station& station : stations){
        uncovered.insert(station.id());
    }


    while(!uncovered.empty()){

        int best_station = -1;
        int best_cover_count = -1;

        for(const Core::Station& station : stations){

            int id = station.id();

            int cover_count = (uncovered.count(id) > 0) ? 1 : 0;
            for(const Core::Edge& edge : graph.neighbors(id)){
                if(uncovered.count(edge.to()) > 0){
                    cover_count++;
                }
            }

            if(cover_count > best_cover_count){

                best_cover_count = cover_count;
                best_station = id;
            }
        }

        result.team_station_ids.push_back(best_station);

        uncovered.erase(best_station);
        for(const Core::Edge& edge : graph.neighbors(best_station)){
            uncovered.erase(edge.to());
        }
    }

    return result;
}

} // namespace QomMetro::Services