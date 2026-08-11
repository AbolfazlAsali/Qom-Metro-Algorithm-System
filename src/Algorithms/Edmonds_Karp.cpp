#include "Algorithms/Edmonds_Karp.hpp"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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


vector<int> Bfs_Find_Path(const Residual_Graph& residual, int source, int sink){

    unordered_map<int, int> predecessor;
    unordered_set<int> visited;
    queue<int> frontier;

    frontier.push(source);
    visited.insert(source);

    while(!frontier.empty()){

        int u = frontier.front();
        frontier.pop();

        if(u == sink){
            break; 
        }

        auto it = residual.find(u);
        if(it == residual.end()){
            continue;
        }

        for(const auto& [v, capacity] : it->second){
            if (capacity > 0.0 && visited.count(v) == 0){

                visited.insert(v);
                predecessor[v] = u;
                frontier.push(v);
            }
        }
    }

    if(visited.count(sink) == 0){
        return {}; 
    }

    vector<int> path;
    int current = sink;

    while(current != source){

        path.push_back(current);
        current = predecessor.at(current);
    }
    path.push_back(source);
    reverse(path.begin(), path.end());
    return path;
}

} // namespace

Max_Flow_Result Edmonds_Karp::run
(
    const Core::IGraph& graph, 
    int source, 
    int sink

) const{


    Max_Flow_Result result;

    if(source == sink){
        return result;
    }

    Residual_Graph residual = Build_Residual_Graph(graph);

    while(true){

        vector<int> path = Bfs_Find_Path(residual, source, sink);
        if(path.empty()){
            break; 
        }

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