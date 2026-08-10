#pragma once

#include "Algorithms/IShortest_Path.hpp"

namespace QomMetro::Algorithms {


class AStar : public IShortest_Path {
public:

    Path_Result run
    (
        const Core::IGraph& graph,
        int from,
        int to,
        Core::Weight_Type weight_type

    ) const override;
};

} // namespace QomMetro::Algorithms