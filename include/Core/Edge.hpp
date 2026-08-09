#pragma once

#include "Core/Weight_Type.hpp"
#include "Utils/Constants.hpp"

namespace QomMetro::Core {

// One directed connection to another station, with the raw travel cost
// (distance and time) plus two optional round-specific overlays:
//   - bonusWeight (Round 2, T2.4): a negative "incentive" adjustment.
//   - capacity (Round 4, T4.2): max passengers per unit time, for max-flow.
// Both default to a neutral value so Round 1 algorithms are unaffected
// unless a later round explicitly sets them.
class Edge {
public:

    Edge(int to, double distance_km, double time_min);

    int to() const;
    double distance_km() const;
    double time_min() const;

    // Base weight used by every Round 1+ routing algorithm (Dijkstra,
    // Prim, Kruskal, ...). Never includes bonusWeight.
    double weight(Weight_Type type) const;

    // Round 2 (T2.4): a negative "incentive" adjustment on some edges.
    // Defaults to 0, so effectiveWeight() == weight() unless explicitly
    // set for a specific analysis scenario.
    double bonus_weight() const;
    void set_bonus_weight(double bonus);

    // weight(type) + bonusWeight(). Used exclusively by BellmanFord
    // (T2.4) to detect negative cycles under the incentive scheme.
    // Round 1 algorithms must never call this.
    double effective_weight(Weight_Type type) const;

    // Round 4 (T4.2): maximum passengers this edge can carry per unit
    // time. Defaults to Utils::kInfinity (unconstrained) until set from
    // capacities.json. Used exclusively by FordFulkerson/EdmondsKarp.
    double capacity() const;
    void set_capacity(double capacity);

private:

    int to_;
    double distance_km_;
    double time_min_;
    double bonus_weight_ = 0.0;
    double capacity_ = Utils::kInfinity;
};

} // namespace QomMetro::Core