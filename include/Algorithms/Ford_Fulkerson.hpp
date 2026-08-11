#pragma once

#include "Algorithms/IMax_Flow.hpp"

namespace QomMetro::Algorithms {


class Ford_Fulkerson : public IMax_Flow {
public:

    Max_Flow_Result run(
        const Core::IGraph& graph, 
        int source, 
        int sink
    ) const override;
};

} // namespace QomMetro::Algorithms
