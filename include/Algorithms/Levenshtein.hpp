#pragma once

#include <string>
#include <vector>

#include "Core/IGraph.hpp"

namespace QomMetro::Algorithms {

// One candidate match from a fuzzy station-name search.
struct Search_Match {

    int station_id;
    std::string station_name;
    int edit_distance;
};


class Levenshtein {
    
public:

    // Minimum number of single-character edits (insert, delete,
    // substitute) needed to turn `a` into `b`.
    static int distance(const std::string& a, const std::string& b);

    // Finds the station(s) in `graph` whose name is closest to `query`.
    // Returns every station tied for the minimum edit distance.
    static std::vector<Search_Match> find_closest(
        const Core::IGraph& graph, const std::string& query);
};

} // namespace QomMetro::Algorithms