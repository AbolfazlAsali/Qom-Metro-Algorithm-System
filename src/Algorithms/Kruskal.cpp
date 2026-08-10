#include "Algorithms/Kruskal.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "Structures/Union_Find.hpp"

using namespace std;

namespace QomMetro::Algorithms {

namespace {


struct Candidate_Edge{
    
    int from;
    int to;
    double weight;
};


vector<Candidate_Edge> Collect_Unique_Edges
(
    const Core::IGraph& graph, 
    Core::Weight_Type weight_type

) {

    vector<Candidate_Edge> edges;

    for(const Core::Station& station : graph.all_stations()){

        int u = station.id();
        for(const Core::Edge& edge : graph.neighbors(u)){

            int v = edge.to();
            if(u < v){
                edges.push_back(Candidate_Edge{u, v, edge.weight(weight_type)});
            }
        }
    }
    return edges;
}

} // namespace

Mst_Result Kruskal::run
(
    const Core::IGraph& graph, 
    Core::Weight_Type weight_type

) const {

     
    Mst_Result result;

    vector<Candidate_Edge> edges = Collect_Unique_Edges(graph, weight_type);

    sort
    (
        edges.begin(), edges.end(),
        [](const Candidate_Edge& a, const Candidate_Edge& b) {
            return a.weight < b.weight;
        }
    );


    unordered_map<int, int> id_to_index;
    int nextIndex = 0;

    for(const Core::Station& station : graph.all_stations()){
        id_to_index[station.id()] = nextIndex++;
    }

    int n = graph.station_count();
    Structures::Union_Find uf(n);
    int edges_used = 0;

    for(const Candidate_Edge& edge : edges){

        int uIndex = id_to_index.at(edge.from);
        int vIndex = id_to_index.at(edge.to);

        if(uf.unite(uIndex, vIndex)){

            result.edges.push_back(Mst_Edge{edge.from, edge.to, edge.weight});
            result.total_weight += edge.weight;
            edges_used++;
        }
    }


    result.success = (edges_used == n - 1);
    return result;
}

} // namespace QomMetro::Algorithms