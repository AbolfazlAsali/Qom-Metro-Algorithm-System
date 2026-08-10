#include "Algorithms/BFS.hpp"

#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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

Reachability_Result Bfs_Reachability::run
(

    const Core::IGraph& graph, 
    int from, 
    int to

) const {



    Reachability_Result result;

    if (from == to){

        result.reachable = true;
        result.path = {from};
        return result;
    }

    queue<int> frontier;
    unordered_set<int> visited;
    unordered_map<int, int> predecessor;

    frontier.push(from);
    visited.insert(from);

    while(!frontier.empty()){

        int current = frontier.front();
        frontier.pop();

        for(const Core::Edge& edge : graph.neighbors(current)){

            int neighbor = edge.to();

            if(visited.count(neighbor) > 0){
                continue;
            }

            visited.insert(neighbor);
            predecessor[neighbor] = current;

            if (neighbor == to){

                result.reachable = true;
                result.path = reconstruct_path(predecessor, from, to);
                return result;
            }

            frontier.push(neighbor);
        }
    }

    result.reachable = false;
    return result;
}

} // namespace QomMetro::Algorithms