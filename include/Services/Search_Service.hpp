#pragma once

#include <string>
#include <vector>

#include "Algorithms/Levenshtein.hpp"
#include "Core/IGraph.hpp"

namespace QomMetro::Services {


class Search_Service {
public:

    std::vector<Algorithms::Search_Match> find_station(
        const Core::IGraph& graph, const std::string& query) const;
};

} // namespace QomMetro::Services