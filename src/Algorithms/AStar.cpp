#include "Algorithms/AStar.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "Structures/Priority_Queue.hpp"

using namespace std;

namespace QomMetro::Algorithms {

namespace {

constexpr double Earth_Radius_Km = 6371.0;

double To_Radians(double degrees){
    return degrees * 3.14159265358979323846 / 180.0;
}


double Haversine_km(const Core::Coordinates& a, const Core::Coordinates& b){

    double lat1 = To_Radians(a.latitude);
    double lat2 = To_Radians(b.latitude);
    double delta_Lat = To_Radians(b.latitude - a.latitude);
    double delta_Lng = To_Radians(b.longitude - a.longitude);

    double sin_Lat = sin(delta_Lat / 2.0);
    double sin_Lng = sin(delta_Lng / 2.0);

    double h = sin_Lat * sin_Lat + cos(lat1) * cos(lat2) * sin_Lng * sin_Lng;
    double c = 2.0 * asin(sqrt(h));

    return Earth_Radius_Km * c;
}


double Fastest_Speed_Km_PerMin(const Core::IGraph& graph){

    double fastest = 0.0;

    for(const Core::Station& station : graph.all_stations()){
        for(const Core::Edge& edge : graph.neighbors(station.id())){

            if(edge.time_min() <= 0.0){
                continue; 
            }
            double speed = edge.distance_km() / edge.time_min();
            fastest = max(fastest, speed);
        }
    }
    return fastest;
}


double heuristic
(
    const Core::IGraph& graph,
    int fromId,
    int toId,
    Core::Weight_Type weight_type,
    double fastest_speed

) {


    const Core::Station& from = graph.station(fromId);
    const Core::Station& to = graph.station(toId);

    if(!from.has_coordinates() || !to.has_coordinates()) {
        return 0.0;
    }

    double Straight_Line_Km = Haversine_km(from.coordinates(), to.coordinates());

    if(weight_type == Core::Weight_Type::Distance){
        return Straight_Line_Km;
    }

    
    if(fastest_speed <= 0.0){
        return 0.0;
    }
    return Straight_Line_Km / fastest_speed;
}

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

Path_Result AStar::run
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

    double fastest_speed = Fastest_Speed_Km_PerMin(graph);


    unordered_map<int, double> dist;
    unordered_map<int, int> predecessor;
    unordered_set<int> finalized;

    Structures::Priority_Queue pq;
    dist[from] = 0.0;
    pq.push_or_decrease(from, heuristic(graph, from, to, weight_type, fastest_speed));

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

                double fScore = candidate + heuristic(graph, neighbor, to, weight_type, fastest_speed);
                pq.push_or_decrease(neighbor, fScore);
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