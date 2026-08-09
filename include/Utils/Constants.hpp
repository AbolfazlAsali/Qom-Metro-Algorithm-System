#pragma once

#include <limits>

namespace QomMetro::Utils {

    // Used by shortest-path algorithms to represent "unreachable" / infinity.
    inline constexpr double kInfinity = std::numeric_limits<double>::infinity();

    // Sentinel value for "no station" / "no predecessor" in path reconstruction.
    inline constexpr int kInvalid_StationId = -1;

    // Default file paths (can be overridden by CLI args later).
    inline constexpr const char* Default_Stations_File = "file_path....";
    inline constexpr const char* Default_Edges_File = "file_path....";
} 