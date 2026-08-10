#include "Algorithms/Dijkstra.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "Structures/Priority_Queue.hpp"

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

Path_Result Dijkstra::run
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

    unordered_map<int, double> dist;
    unordered_map<int, int> predecessor;
    unordered_set<int> finalized;

    Structures::Priority_Queue pq;
    dist[from] = 0.0;
    pq.push_or_decrease(from, 0.0);

    while(!pq.empty()){

        int current = pq.pop_min();
        finalized.insert(current);
        result.nodes_expanded++;

        if(current == to){
            break; 
        }

        double current_dist = dist.at(current);

        for(const Core::Edge& edge : graph.neighbors(current)){

            int neighbor = edge.to();

            if(finalized.count(neighbor) > 0){
                continue; 
            }

            double candidate = current_dist + edge.weight(weight_type);

            auto it = dist.find(neighbor);
            if(it == dist.end() || candidate < it->second){

                dist[neighbor] = candidate;
                predecessor[neighbor] = current;
                pq.push_or_decrease(neighbor, candidate);
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