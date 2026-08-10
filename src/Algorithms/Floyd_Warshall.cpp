#include "Algorithms/Floyd_Warshall.hpp"

#include <limits>

#include "Utils/Constants.hpp"

using namespace std;

namespace QomMetro::Algorithms {

void Floyd_Warshall::build(const Core::IGraph& graph, Core::Weight_Type weight_type){

    const vector<Core::Station>& stations = graph.all_stations();
    int n = static_cast<int>(stations.size());

   
    station_ids_.clear();
    id_to_index_.clear();
    station_ids_.reserve(n);

    for(int i = 0; i < n; ++i){

        int id = stations[i].id();
        station_ids_.push_back(id);
        id_to_index_[id] = i;
    }


    dist_.assign(n, vector<double>(n, Utils::kInfinity));
    next_hop_.assign(n, vector<int>(n, -1));

    for(int i = 0; i < n; ++i){

        dist_[i][i] = 0.0;
        next_hop_[i][i] = i;
    }

    for(int i = 0; i < n; ++i){

        int from_id = station_ids_[i];
        for(const Core::Edge& edge : graph.neighbors(from_id)){

            int j = id_to_index_.at(edge.to());
            double w = edge.weight(weight_type);


            if(w < dist_[i][j]){

                dist_[i][j] = w;
                next_hop_[i][j] = j;
            }
        }
    }

    for(int k = 0; k < n; ++k){

        for(int i = 0; i < n; ++i){

            if(dist_[i][k] == Utils::kInfinity){
                continue; 
            }

            for(int j = 0; j < n; ++j){

                if(dist_[k][j] == Utils::kInfinity){
                    continue;
                }

                double candidate = dist_[i][k] + dist_[k][j];

                if(candidate < dist_[i][j]){

                    dist_[i][j] = candidate;
                    next_hop_[i][j] = next_hop_[i][k];
                }
            }
        }
    }
}



Path_Result Floyd_Warshall::query(int from, int to) const {


    Path_Result result;

    auto from_It = id_to_index_.find(from);
    auto to_It = id_to_index_.find(to);

    if(from_It == id_to_index_.end() || to_It == id_to_index_.end()){

        result.reachable = false;
        return result;
    }

    int i = from_It->second;
    int j = to_It->second;

    if(dist_[i][j] == Utils::kInfinity){

        result.reachable = false;
        return result;
    }

    result.reachable = true;
    result.total_cost = dist_[i][j];

    
    vector<int> path;
    int current = i;
    path.push_back(station_ids_[current]);

    while(current != j){

        current = next_hop_[current][j];
        path.push_back(station_ids_[current]);
    }


    result.path = move(path);
    return result;
}

} // namespace QomMetro::Algorithms