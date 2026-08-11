#include "Algorithms/Prim.hpp"

#include <unordered_map>
#include <unordered_set>

#include "Structures/Priority_Queue.hpp"

using namespace std;

namespace QomMetro::Algorithms {

Mst_Result Prim::run(const Core::IGraph& graph, Core::Weight_Type weight_type) const{


    Mst_Result result;

    const vector<Core::Station>& stations = graph.all_stations();
    int n = static_cast<int>(stations.size());

    if(n == 0){

        result.success = true; 
        return result;
    }

    unordered_set<int> In_Tree;

   
    unordered_map<int, Core::Edge> best_edge_to_tree;
    unordered_map<int, int> best_edge_source;

   
    for(const Core::Station& start_station : stations){

        int startId = start_station.id();
        if(In_Tree.count(startId) > 0){
            continue; 
        }

        Structures::Priority_Queue pq;
        pq.push_or_decrease(startId, 0.0);
        bool Is_first_In_component = true;

        while(!pq.empty()){

            int u = pq.pop_min();

            if(In_Tree.count(u) > 0){
                continue;
            }
            In_Tree.insert(u);


            if(Is_first_In_component){     
                Is_first_In_component = false;
            } 
            else{

                const Core::Edge& chosen = best_edge_to_tree.at(u);
                int source = best_edge_source.at(u);
                double w = chosen.weight(weight_type);
                result.edges.push_back(Mst_Edge{source, u, w});
                result.total_weight += w;
            }

            for(const Core::Edge& edge : graph.neighbors(u)){

                int v = edge.to();
                if(In_Tree.count(v) > 0){
                    continue;
                }

                double w = edge.weight(weight_type);
                auto it = best_edge_to_tree.find(v);

                if(it == best_edge_to_tree.end() || w < it->second.weight(weight_type)){
                    
                    best_edge_to_tree.insert_or_assign(v, edge);
                    best_edge_source[v] = u;
                    pq.push_or_decrease(v, w);
                }
            }
        }
    }

   
    result.success = (static_cast<int>(In_Tree.size()) == n);
    return result;
}

} // namespace QomMetro::Algorithms