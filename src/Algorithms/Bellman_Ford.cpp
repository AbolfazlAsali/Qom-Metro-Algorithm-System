#include "Algorithms/Bellman_Ford.hpp"

#include <algorithm>
#include <unordered_map>

using namespace std;

namespace QomMetro::Algorithms {

namespace {

vector<int> reconstruct_path
(
    const unordered_map<int, int>& predecessor, 
    int from, 
    int to
) {

    vector<int> path;
    int current = to;

    while(current != from){

        path.push_back(current);
        current = predecessor.at(current);
    }

    path.push_back(from);
    reverse(path.begin(), path.end());
    return path;
}

} // namespace

Path_Result Bellman_Ford::run
(
    const Core::IGraph& graph, 
    int from, 
    int to, 
    Core::Weight_Type weight_type

) const {

    
    Path_Result result;

    unordered_map<int, double> dist;
    unordered_map<int, int> predecessor;
    dist[from] = 0.0;

    int n = graph.station_count();

   
    for(int iteration = 0; iteration < n - 1; ++iteration){

        bool any_change = false;

        for(const Core::Station& station : graph.all_stations()){

            int u = station.id();
            auto uIt = dist.find(u);

            if(uIt == dist.end()){
                continue; 
            }


            double uDist = uIt->second;

            for(const Core::Edge& edge : graph.neighbors(u)){

                int v = edge.to();
                double candidate = uDist + edge.effective_weight(weight_type);

                auto vIt = dist.find(v);
                if(vIt == dist.end() || candidate < vIt->second){

                    dist[v] = candidate;
                    predecessor[v] = u;
                    any_change = true;
                }
            }
        }

        if (!any_change) {
            break;
        }
    }

    
    for(const Core::Station& station : graph.all_stations()){

        int u = station.id();
        auto uIt = dist.find(u);
        
        if(uIt == dist.end()){
            continue;
        }

        
        double uDist = uIt->second;

        for(const Core::Edge& edge : graph.neighbors(u)){

            int v = edge.to();
            double candidate = uDist + edge.effective_weight(weight_type);

            auto vIt = dist.find(v);
            if(vIt == dist.end() || candidate < vIt->second){

                result.has_negative_cycle = true;
                return result;
            }
        }
    }

    auto it = dist.find(to);
    if(it == dist.end()){

        result.reachable = false;
        return result;
    }

    result.reachable = true;
    result.total_cost = it->second;
    result.path = reconstruct_path(predecessor, from, to);
    return result;
}

} // namespace QomMetro::Algorithms