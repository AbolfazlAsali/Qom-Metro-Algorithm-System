#include "Algorithms/DAG_Shortest_Path.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

namespace QomMetro::Algorithms {

namespace {


void topo_visit
(
    const Core::IGraph& graph,
    int u,
    unordered_set<int>& visited,
    vector<int>& order
) {

    visited.insert(u);
    for(const Core::Edge& edge : graph.neighbors(u)){

        int v = edge.to();
        if(visited.count(v) == 0){
            topo_visit(graph, v, visited, order);
        }
    }
    order.push_back(u);
}

vector<int> topological_sort(const Core::IGraph& graph){

    unordered_set<int> visited;
    vector<int> order;

    for(const Core::Station& station : graph.all_stations()){

        int id = station.id();
        if(visited.count(id) == 0){
            topo_visit(graph, id, visited, order);
        }
    }

    reverse(order.begin(), order.end());
    return order;
}

vector<int> reconstruct_path
(
    const unordered_map<int, int>& predecessor, 
    int from, 
    int to
){

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

Path_Result Dag_Shortest_Path::run
(
    const Core::IGraph& graph, 
    int from, 
    int to, 
    Core::Weight_Type weight_type

) const {


    Path_Result result;

    if(from == to){

        result.reachable = true;
        result.path = {from};
        result.total_cost = 0.0;
        return result;
    }

    vector<int> order = topological_sort(graph);

    unordered_map<int, double> dist;
    unordered_map<int, int> predecessor;
    dist[from] = 0.0;


    for(int u : order){

        auto uIt = dist.find(u);
        if(uIt == dist.end()){
            continue; 
        }

        double uDist = uIt->second;

        for(const Core::Edge& edge : graph.neighbors(u)){

            int v = edge.to();
            double candidate = uDist + edge.weight(weight_type);

            auto vIt = dist.find(v);
            if(vIt == dist.end() || candidate < vIt->second){

                dist[v] = candidate;
                predecessor[v] = u;
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