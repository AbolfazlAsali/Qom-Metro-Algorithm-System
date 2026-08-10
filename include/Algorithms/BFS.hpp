#pragma once

#include "Algorithms/IReachability.hpp"

namespace QomMetro::Algorithms{


class Bfs_Reachability : public IReachability {

public:

    Reachability_Result run(

        const Core::IGraph& graph, 
        int from, 
        int to
    
    ) const override;
};

} // namespace QomMetro::Algorithms