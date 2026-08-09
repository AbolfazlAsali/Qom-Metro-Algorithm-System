#pragma once

namespace QomMetro::Core {

// The criterion used to weigh an edge when computing a route: either
// physical distance (km) or travel time (minutes). Passed explicitly into
// every shortest-path algorithm so the caller (a user via the CLI, or a
// Service) decides which one matters for a given query.
enum class Weight_Type {
    Distance,
    Time
};

} // namespace QomMetro::Core