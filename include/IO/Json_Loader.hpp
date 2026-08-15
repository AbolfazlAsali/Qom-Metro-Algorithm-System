#pragma once

#include <string>
#include <vector>

#include "IO/File_Loader.hpp"
#include "Simulation/Platform_Allocator.hpp"
#include "Simulation/Statistics.hpp"

namespace QomMetro::IO {

// Expected schema:
//
//   stations.json:
//   { "stations": [
//       { "id": 0, "name": "...", "lat": 34.64, "lng": 50.87 },  // lat/lng optional
//       { "id": 1, "name": "..." }
//   ]}
//
//   edges.json / express_graph.json:
//   { "edges": [
//       { "from": 0, "to": 1, "distance_km": 2.5, "time_min": 5 }
//   ]}
//   edges.json is loaded bidirectionally by loadGraph() (the main metro
//   graph is undirected); express_graph.json is loaded directed-only by
//   loadDirectedGraph() (the express line is one-way -- see README.md).
//
//   capacities.json (T4.2, optional):
//   { "capacities": [
//       { "from": 0, "to": 1, "capacity": 800 }
//   ]}
//   Each entry sets the capacity of ONE directed edge (from -> to).
//   Capacity is independent per direction (see README.md, "Design
//   Decisions"), so an entry for the reverse direction (to -> from)
//   must be listed separately if it should be limited too.
//
//   incentives.json (T2.4, optional):
//   { "incentives": [
//       { "from": 0, "to": 1, "bonus": -1.5 }
//   ]}
//   Same one-directional convention as capacities.json, and for the same
//   reason -- see README.md on why incentive weights must not be applied
//   symmetrically.
//
//   trains.json (T3.1/T3.2, optional):
//   { "trains": [
//       { "id": 1, "arrival_min": 10, "departure_min": 15, "delay_min": 0 }
//   ]}
//   "delay_min" is only used by the dispatch-queue demo (T3.2), not by
//   platform allocation (T3.1) -- see Simulation::Train.
//
//   passengers.json (T3.3, optional):
//   { "visits": [
//       { "station_id": 0, "day": 1 }
//   ]}
class Json_Loader : public File_Loader{

public:

    std::unique_ptr<Core::Graph> load_Graph(

        const std::string& stations_file_path,
        const std::string& edges_file_path
    
    ) const override;



    void load_Capacities(

        const std::string& capacities_file_path,
        Core::Graph& graph
    
    ) const override;



    void load_Incentives(

        const std::string& incentives_file_path,
        Core::Graph& graph
    
    ) const override;



    std::unique_ptr<Core::Graph> load_Directed_Graph(

        const std::string& stations_file_path,
        const std::string& edges_file_path
    
    ) const override;


    std::vector<Simulation::Train> load_Trains(const std::string& path) const;
    std::vector<Simulation::Visit_Record> load_Visits(const std::string& path) const;
};

} // namespace QomMetro::IO