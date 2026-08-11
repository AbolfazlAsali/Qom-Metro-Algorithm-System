#pragma once

#include <optional>

#include "Structures/Priority_Queue.hpp"

namespace QomMetro::Simulation {


class Dispatch_Queue_Manager {
public:

    // Adds a train to the dispatch queue, or lowers its priority if it's
    // already waiting with a worse (larger) one.
    void add_or_update(int trainId, double priority);

    // Removes and returns the id of the highest-priority (smallest
    // value) waiting train. Returns std::nullopt if no train is waiting.
    std::optional<int> dispatch_next();

    bool empty();

private:
    Structures::Priority_Queue queue_;
};

} // namespace QomMetro::Simulation