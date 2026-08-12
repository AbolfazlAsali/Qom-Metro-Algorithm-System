#include "Services/Search_Service.hpp"

namespace QomMetro::Services {

std::vector<Algorithms::Search_Match> Search_Service::find_station(
    const Core::IGraph& graph, const std::string& query) const {
    return Algorithms::Levenshtein::find_closest(graph, query);
}

} // namespace QomMetro::Services