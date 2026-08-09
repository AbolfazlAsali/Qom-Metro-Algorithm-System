#pragma once

#include <unordered_map>
#include "Structures/Min_Heap.hpp"

namespace QomMetro::Structures {


class Priority_Queue {

public:

    Priority_Queue() = default;

    // Inserts id with the given priority, or lowers its priority if id is
    // already present with a worse (larger) one. Does nothing if id is
    // already present with an equal or better priority.
    void push_or_decrease(int id, double priority);

    // Removes and returns the id with the smallest current priority.
    int pop_min();
    bool empty();

private:

    Min_Heap heap_;
    std::unordered_map<int, double> best_priority_;

    void discard_stale_top();
};

} // namespace QomMetro::Structures