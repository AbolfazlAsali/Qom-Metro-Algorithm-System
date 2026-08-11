#pragma once

#include "Algorithms/IMst.hpp"

namespace QomMetro::Algorithms {

class Prim : public IMst {
public:

    Mst_Result run(
        const Core::IGraph& graph, 
        Core::Weight_Type weight_type
    ) const override;
};

} // namespace QomMetro::Algorithms