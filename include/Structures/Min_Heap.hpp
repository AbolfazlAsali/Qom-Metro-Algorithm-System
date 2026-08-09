#pragma once

#include <cstddef>
#include <vector>

namespace QomMetro::Structures {


struct Heap_Item {

    double priority;
    int id;
};


class Min_Heap{

public:

    Min_Heap() = default;

    void push(double priority, int id);
    Heap_Item extract_min();
    const Heap_Item& peek() const;

    bool empty() const;
    std::size_t size() const;

private:

    std::vector<Heap_Item> data_;

    void heapUp(std::size_t index);
    void heapDown(std::size_t index);

    std::size_t parent(std::size_t index) const;
    std::size_t left_Child(std::size_t index) const;
    std::size_t right_Child(std::size_t index) const;
};

} // namespace QomMetro::Structures