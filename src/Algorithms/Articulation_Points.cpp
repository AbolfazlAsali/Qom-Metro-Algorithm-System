#include "Algorithms/Articulation_Points.hpp"

#include <algorithm>

#include "Utils/Constants.hpp"

using namespace std;

namespace QomMetro::Algorithms {

void Articulation_Points::dfs
(
    const Core::IGraph& graph, 
    int u, 
    int parent, 
    Dfs_State& state

) const {


    state.discovery_time[u] = state.low[u] = state.timer++;
    int children = 0;

    for(const Core::Edge& edge : graph.neighbors(u)){

        int v = edge.to();

        if(v == parent){
            continue; 
        }

        if(state.discovery_time.count(v) == 0){
           
            children++;
            dfs(graph, v, u, state);

            state.low[u] = min(state.low[u], state.low[v]);

            
            bool uIs_Root = (parent == Utils::kInvalid_StationId);
            if(!uIs_Root && state.low[v] >= state.discovery_time[u]){
                state.Is_Articulation.insert(u);
            }


            if(state.low[v] > state.discovery_time[u]){
                state.bridges.push_back(Bridge{u, v});
            }
        } 
        else{
            state.low[u] = min(state.low[u], state.discovery_time[v]);
        }
    }

    if(parent == Utils::kInvalid_StationId && children > 1){
        state.Is_Articulation.insert(u);
    }
}

Articulation_Result Articulation_Points::run(const Core::IGraph& graph) const {

    Dfs_State state;

    for(const Core::Station& station : graph.all_stations()){

        int id = station.id();
        if(state.discovery_time.count(id) == 0){
            dfs(graph, id, Utils::kInvalid_StationId, state);
        }
    }

    Articulation_Result result;
    result.articulation_points.assign(
        state.Is_Articulation.begin(), state.Is_Articulation.end());


    result.bridges = move(state.bridges);
    return result;
}

} // namespace QomMetro::Algorithms