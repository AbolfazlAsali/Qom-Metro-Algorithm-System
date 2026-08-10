#include "Algorithms/DFS.hpp"

#include <algorithm>

using namespace std;

namespace QomMetro::Algorithms {

namespace {

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

bool Dfs_Reachability::visit
(
    const Core::IGraph& graph,
    int current,
    int to,
    unordered_set<int>& visited,
    unordered_map<int, int>& predecessor

) const {



    if(current == to){
        return true;
    }

    for(const Core::Edge& edge : graph.neighbors(current)){

        int neighbor = edge.to();

        if(visited.count(neighbor) > 0){
            continue;
        }

        visited.insert(neighbor);
        predecessor[neighbor] = current;

        if(visit(graph, neighbor, to, visited, predecessor)){
            return true;
        }
    }

    return false; 
}

Reachability_Result Dfs_Reachability::run
(
    const Core::IGraph& graph, 
    int from, 
    int to

) const {



    Reachability_Result result;

    if(from == to){

        result.reachable = true;
        result.path = {from};
        return result;
    }

    unordered_set<int> visited;
    unordered_map<int, int> predecessor;
    visited.insert(from);

    if(visit(graph, from, to, visited, predecessor)){

        result.reachable = true;
        result.path = reconstruct_path(predecessor, from, to);
    } 
    else{
        result.reachable = false;
    }

    return result;
}

} // namespace QomMetro::Algorithms