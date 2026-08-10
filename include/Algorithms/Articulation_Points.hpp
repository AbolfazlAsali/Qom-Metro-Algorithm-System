#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Core/IGraph.hpp"

namespace QomMetro::Algorithms {

// A cut edge: removing it disconnects the graph.
struct Bridge {
    int from;
    int to;
};

// Result of an articulation-point / bridge analysis.
struct Articulation_Result {

    // Station ids whose removal disconnects part of the network.
    std::vector<int> articulation_points;
    std::vector<Bridge> bridges;
};


class Articulation_Points {

public:

    Articulation_Result run(const Core::IGraph& graph) const;

private:

    struct Dfs_State{

        std::unordered_map<int, int> discovery_time;
        std::unordered_map<int, int> low;
        std::unordered_set<int> Is_Articulation;
        std::vector<Bridge> bridges;
        int timer = 0;
    };

    // Explores from `u`, having arrived via `parent` (Utils::kInvalid_StationId
    // if `u` is the root of its DFS tree).
    void dfs(const Core::IGraph& graph, int u, int parent, Dfs_State& state) const;
};

} // namespace QomMetro::Algorithms