#pragma once

#include <vector>

namespace QomMetro::Structures {

class Union_Find{

public:

    explicit Union_Find(int size);

    // Returns the representative (root) of the set containing x.
    // Applies path compression as a side effect.
    int find(int x);

    // Merges the sets containing a and b. Returns false if they were
    // already in the same set (i.e. adding this edge would form a cycle).
    bool unite(int a, int b);

    // Returns true if a and b are already in the same set.
    bool connected(int a, int b);

private:

    std::vector<int> parent_;
    std::vector<int> rank_;
};

} // namespace QomMetro::Structures