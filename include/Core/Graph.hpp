#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Core/Edge.hpp"
#include "Core/IGraph.hpp"
#include "Core/Station.hpp"

namespace QomMetro::Core {


class Graph : public IGraph{

public:

    Graph() = default;

   
    void add_station(Station station);
    void add_edge(int from, Edge edge);


    bool set_edge_capacity(int from, int to, double capacity);
    bool set_edge_bonus(int from, int to, double bonus);

    // --- IGraph interface ---
    int station_count() const override;
    bool has_station(int stationId) const override;
    const Station& station(int stationId) const override;
    const std::vector<Edge>& neighbors(int stationId) const override;
    const std::vector<Station>& all_stations() const override;
    int find_StationId_ByName(const std::string& name) const override;

private:

    std::vector<Station> stations_;                 // preserves load order, for allStations()
    std::unordered_map<int, std::size_t> id_to_index_; // stationId -> index into stations_
    std::unordered_map<int, std::vector<Edge>> adjacency_; // stationId -> outgoing edges
};

} // namespace QomMetro::Core