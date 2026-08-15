#pragma once

#include <limits>

namespace QomMetro::Utils {

    // Used by shortest-path algorithms to represent "unreachable" / infinity.
    inline constexpr double kInfinity = std::numeric_limits<double>::infinity();

    // Sentinel value for "no station" / "no predecessor" in path reconstruction.
    inline constexpr int kInvalid_StationId = -1;

    // Default file paths (can be overridden by CLI args later).
    inline constexpr const char* Default_Stations_File = "data/stations.json";
    inline constexpr const char* Default_Edges_File = "data/edges.json";
    inline constexpr const char* Default_Capacities_File = "data/capacities.json";
    inline constexpr const char* Default_Incentives_File = "data/incentives.json";
    inline constexpr const char* Default_ExpressEdges_File = "data/express_graph.json";
    inline constexpr const char* Default_Trains_File = "data/trains.json";
    inline constexpr const char* Default_Passengers_File = "data/passengers.json";
} 