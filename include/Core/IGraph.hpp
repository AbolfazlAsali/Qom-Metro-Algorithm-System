#pragma once

#include <string>
#include <vector>

#include "Core/Edge.hpp"
#include "Core/Station.hpp"

namespace QomMetro::Core {

// Read-only view of the metro network graph. Every algorithm in the
// project (BFS, Dijkstra, Kruskal, ...) depends only on this interface,
// never on a concrete graph implementation -- so a new graph
// representation (e.g. an adjacency matrix, for comparison in the report)
// can be added later without touching a single algorithm.
//
// Deliberately has no mutating methods (no addStation/addEdge): only the
// data loader needs to build a graph, and giving algorithms write access
// they never use would violate Interface Segregation.

class IGraph{

public:

    virtual ~IGraph() = default;

    // Total number of stations in the graph.
    virtual int station_count() const = 0;

    // True if a station with this id exists.
    virtual bool has_station(int stationId) const = 0;

    // The Station object for this id.
    // Precondition: has_station(stationId) must be true.
    virtual const Station& station(int stationId) const = 0;

    // All outgoing edges from this station. Returns an empty vector if the
    // station has no outgoing edges (never null, never throws for a valid id).
    virtual const std::vector<Edge>& neighbors(int stationId) const = 0;

    // All stations in the graph, in an unspecified but stable order.
    // Needed for tasks that must scan every station
    virtual const std::vector<Station>& all_stations() const = 0;

    // Finds a station by exact name match. Returns
    // Utils::kInvalid_StationId if no station has this name.
    virtual int find_StationId_ByName(const std::string& name) const = 0;
};

} // namespace QomMetro::Core