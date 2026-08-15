#include "Core/Graph.hpp"

#include <cassert>
#include <utility>

#include "Utils/Constants.hpp"

namespace QomMetro::Core {

void Graph::add_station(Station station){

    int id = station.id();
    id_to_index_[id] = stations_.size();
    stations_.push_back(std::move(station));

    adjacency_[id];
}

void Graph::add_edge(int from, Edge edge){

    assert(has_station(from) && "addEdge: 'from' station does not exist");
    assert(has_station(edge.to()) && "addEdge: 'to' station does not exist");
    adjacency_[from].push_back(std::move(edge));
}


bool Graph::set_edge_capacity(int from, int to, double capacity){

    auto it = adjacency_.find(from);
    if(it == adjacency_.end()){
        return false;
    }
    for(Edge& edge : it->second){
        if(edge.to() == to){
            edge.set_capacity(capacity);
            return true;
        }
    }
    return false;
}

bool Graph::set_edge_bonus(int from, int to, double bonus){

    auto it = adjacency_.find(from);
    if(it == adjacency_.end()){
        return false;
    }
    for(Edge& edge : it->second){
        if(edge.to() == to){
            edge.set_bonus_weight(bonus);
            return true;
        }
    }
    return false;
}



int Graph::station_count() const{
    return static_cast<int>(stations_.size());
}

bool Graph::has_station(int stationId) const{
    return id_to_index_.find(stationId) != id_to_index_.end();
}

const Station& Graph::station(int stationId) const{

    auto it = id_to_index_.find(stationId);
    assert(it != id_to_index_.end() && "station(): unknown stationId");
    return stations_[it->second];
}

const std::vector<Edge>& Graph::neighbors(int stationId) const{

    static const std::vector<Edge> kempty;
    auto it = adjacency_.find(stationId);
    if (it == adjacency_.end()) {
        return kempty;
    }
    return it->second;
}

const std::vector<Station>& Graph::all_stations() const{
    return stations_;
}

int Graph::find_StationId_ByName(const std::string& name) const{

    for(const Station& s : stations_){
        if(s.name() == name){
            return s.id();
        }
    }
    return Utils::kInvalid_StationId;
}

} // namespace QomMetro::Core