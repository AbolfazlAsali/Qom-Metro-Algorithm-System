#pragma once

#include <unordered_map>
#include <vector>

#include "Algorithms/IShortest_Path.hpp"
#include "Core/IGraph.hpp"
#include "Core/Weight_Type.hpp"

namespace QomMetro::Algorithms {


class Floyd_Warshall {

public:

    // Runs the all-pairs computation once. Must be called before
    // any query().
    void build(const Core::IGraph& graph, Core::Weight_Type weight_type);

    // Answers a single from/to query using the matrices computed by the
    // most recent build().
    Path_Result query(int from, int to) const;

private:

    std::vector<int> station_ids_;             
    std::unordered_map<int, int> id_to_index_;  

    std::vector<std::vector<double>> dist_;  
    std::vector<std::vector<int>> next_hop_;  
};

} // namespace QomMetro::Algorithms