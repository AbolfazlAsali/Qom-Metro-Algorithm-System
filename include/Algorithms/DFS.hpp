#pragma once

#include <unordered_map>
#include <unordered_set>

#include "Algorithms/IReachability.hpp"

namespace QomMetro::Algorithms {



class Dfs_Reachability : public IReachability {

public:

    Reachability_Result run(

        const Core::IGraph& graph, 
        int from, 
        int to
    
    ) const override;

private:
    
    bool visit
    (
        const Core::IGraph& graph,
        int current,
        int to,
        std::unordered_set<int>& visited,
        std::unordered_map<int, int>& predecessor
    
    ) const;
};

} // namespace QomMetro::Algorithms