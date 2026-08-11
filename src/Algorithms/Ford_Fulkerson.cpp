#include "Algorithms/Ford_Fulkerson.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Utils/Constants.hpp"

using namespace std;

namespace QomMetro::Algorithms {

namespace {


constexpr double kLarge_capacity = 1e15;

using Residual_Graph = unordered_map<int, unordered_map<int, double>>;

Residual_Graph Build_Residual_Graph(const Core::IGraph& graph){


    Residual_Graph residual;

    for(const Core::Station& station : graph.all_stations()){

        int u = station.id();
        for(const Core::Edge& edge : graph.neighbors(u)){

            int v = edge.to();
            double cap = edge.capacity();

            residual[u][v] = (cap == Utils::kInfinity) ? kLarge_capacity : cap;
           
            residual[v][u] += 0.0;
        }
    }

    return residual;
}


bool Dfs_Find_Path
(

    const Residual_Graph& residual,
    int u,
    int sink,
    unordered_set<int>& visited,
    vector<int>& path

) {


    if(u == sink){
        path.push_back(u);
        return true;
    }
    visited.insert(u);

    
    auto it = residual.find(u);
    if(it == residual.end()){
        return false;
    }



    for(const auto& [v, capacity] : it->second){

        if(capacity > 0.0 && visited.count(v) == 0){
            if(Dfs_Find_Path(residual, v, sink, visited, path)){

                path.push_back(u);
                return true;
            }
        }
    }

    return false;
}

} // namespace


Max_Flow_Result Ford_Fulkerson::run(const Core::IGraph& graph, int source, int sink) const{


    Max_Flow_Result result;

    if(source == sink){
        return result; 
    }


    Residual_Graph residual = Build_Residual_Graph(graph);

    while(true){

        unordered_set<int> visited;
        vector<int> path;

        if(!Dfs_Find_Path(residual, source, sink, visited, path)){
            break; 
        }

        reverse(path.begin(), path.end()); 
        double bottleneck = kLarge_capacity;

        for(size_t i = 0; i + 1 < path.size(); ++i){
            bottleneck = min(bottleneck, residual[path[i]][path[i + 1]]);
        }

        for(size_t i = 0; i + 1 < path.size(); ++i){

            int a = path[i];
            int b = path[i + 1];
            residual[a][b] -= bottleneck;
            residual[b][a] += bottleneck;
        }

        result.max_flow += bottleneck;
    }

    return result;
}

} // namespace QomMetro::Algorithms