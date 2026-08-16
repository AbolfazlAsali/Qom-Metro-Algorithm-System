# From Qom to New York — Qom Metro Algorithmic System

Final project for the Algorithm Design course | Bu-Ali Sina University | Spring 1405 (2026)

---

## 1. Project Overview

This project simulates a metro network management system for the city of Qom, built
incrementally over 5 rounds. Every round builds on the graph infrastructure created in
Round 1; no part of the system is implemented as a standalone/disconnected module — all
algorithms operate on a single shared `IGraph`.

**Implementation language:** C++ (C++17 or later)
**Paradigm:** Object-Oriented Programming, following SOLID principles as closely as possible
**Build system:** CMake

---

## 2. Design Principles (SOLID, applied to this project)

| Principle | Practical meaning here |
|---|---|
| **S** — Single Responsibility | `Graph` only holds data, each algorithm gets its own class, each `Service` owns one concern, `Loader` only reads files. |
| **O** — Open/Closed | New algorithm = new class behind an existing interface; old code stays untouched (e.g. adding `AStar` in Round 5 without modifying `Dijkstra`). |
| **L** — Liskov Substitution | Every implementation of `IGraph` / `IShortestPath` / `IMst` / ... must honor the same contract (e.g. always return an empty list instead of null). |
| **I** — Interface Segregation | Instead of one giant `IAlgorithm`, each algorithm family (shortest-path, MST, max-flow, reachability, ...) has its own narrow interface. |
| **D** — Dependency Inversion | High-level modules (`MetroSystem`, `Services/*`) depend on interfaces (`IGraph`, `IShortestPath`, ...), never on a concrete class. |

Rule of thumb while coding: ask yourself "how many reasons does this class have to
change?" and "if a new algorithm is added tomorrow, do I have to touch old code?".

---

## 3. Folder Structure

```
QomMetro/
├── CMakeLists.txt
├── COMMITS.md
│
├── docs/
│   ├── architecture.md          (this file)
│   └── report.md
│
├── data/
│   ├── stations.json
│   ├── edges.json
│   ├── express_graph.json       (T2.3 — one-way express line)
│   ├── capacities.json          (T4.2 — edge capacities)
│   ├── incentives.json          (T2.4 — one-directional bonus weights)
│   ├── trains.json              (T3.1 / T3.2)
│   └── passengers.json          (T3.3)
│
├── include/
│   │
│   ├── Core/
│   │   ├── MetroSystem.hpp             (Facade)
│   │   ├── IGraph.hpp
│   │   ├── Graph.hpp
│   │   ├── Station.hpp
│   │   ├── Edge.hpp
│   │   └── WeightType.hpp
│   │
│   ├── Algorithms/
│   │   ├── IReachability.hpp
│   │   ├── BFS.hpp
│   │   ├── DFS.hpp
│   │   ├── IShortestPath.hpp
│   │   ├── Dijkstra.hpp
│   │   ├── AStar.hpp                   (Round 5, Track A)
│   │   ├── BellmanFord.hpp
│   │   ├── FloydWarshall.hpp
│   │   ├── DAGShortestPath.hpp
│   │   ├── IMst.hpp
│   │   ├── Kruskal.hpp
│   │   ├── Prim.hpp
│   │   ├── IMaxFlow.hpp
│   │   ├── FordFulkerson.hpp
│   │   ├── EdmondsKarp.hpp
│   │   ├── ArticulationPoints.hpp
│   │   └── Levenshtein.hpp
│   │
│   ├── Structures/
│   │   ├── UnionFind.hpp
│   │   ├── PriorityQueue.hpp
│   │   └── MinHeap.hpp
│   │
│   ├── Simulation/
│   │   ├── PlatformAllocator.hpp       (T3.1 — interval scheduling)
│   │   ├── DispatchQueueManager.hpp    (T3.2 — priority queue)
│   │   ├── PassengerSimulator.hpp      (T3.4)
│   │   └── Statistics.hpp              (T3.3)
│   │
│   ├── Services/
│   │   ├── RoutingService.hpp          (wraps IReachability/IShortestPath)
│   │   ├── NetworkService.hpp          (MST, max-flow, connectivity)
│   │   ├── AnalysisService.hpp         (Floyd-Warshall, analytics, T4.4)
│   │   └── SearchService.hpp           (Levenshtein station search)
│   │
│   ├── IO/
│   │   ├── FileLoader.hpp              (interface)
│   │   └── JsonLoader.hpp
│   │
│   └── Utils/
│       ├── Logger.hpp
│       ├── Timer.hpp                   (benchmarking, used for algorithm comparisons)
│       └── Constants.hpp
│
├── src/
│   ├── Core/
│   ├── Algorithms/
│   ├── Structures/
│   ├── Simulation/
│   ├── Services/
│   ├── IO/
│   └── Utils/
│       (each mirrors its include/ counterpart — one .cpp per .hpp
│        that has real logic; pure interfaces have no .cpp at all)
│
└── cli/
    └── main.cpp                        (composition root; not under src/)
```

**Note on what this project deliberately does NOT have:** no
`third_party/` (the `nlohmann/json` dependency is fetched via CMake's
`FetchContent` at configure time, never vendored as a file in the
repo), no `tests/` (see Section 8 — kept out of this delivery, with
correctness instead verified via manual CLI runs and cross-checking
paired algorithms), no `output/` or `scripts/` (both were part of an
earlier, more elaborate proposed structure that was simplified away
since nothing in the actual build/run workflow needs them), and no
`LICENSE`/`.gitignore` (neither required by the assignment).

**Golden rule:** no algorithm talks to `Graph` directly; everything goes through
`IGraph`, so Round 5 (innovation) can add `AStar` without touching the graph or
existing algorithms. Likewise, each `Services/*` class talks to algorithms only
through their interfaces (`IShortestPath`, `IMst`, `IMaxFlow`, `IReachability`),
never through a concrete algorithm class.

---

## 4. Core Data Model (Round 1 decisions)

- **`Station`**: `id` (int) + `name` (string) + `latitude`/`longitude` (double,
  optional at first — needed later for the `AStar` heuristic in Round 5, added now
  so `Station` doesn't need to change again in Round 5).
- **`Edge`**: `to`, `distanceKm`, `timeMin`, plus two optional round-specific
  overlays that default to a neutral value so Round 1 algorithms are unaffected:
  `bonusWeight` (Round 2, T2.4, negative "incentive" weight) and `capacity`
  (Round 4, T4.2, max passengers/unit time for max-flow, defaults to infinity).
- **`WeightType`**: `enum class { Distance, Time }` — the caller picks the routing
  criterion.
- **Graph representation**: adjacency list (`unordered_map<int, vector<Edge>>`) since
  the graph is sparse (edge count ≪ n²). This decision, and its comparison against an
  adjacency matrix, must be justified in the technical report.
- **Directedness: the graph is UNDIRECTED.** A metro line can be traveled in
  either direction, so `JsonLoader` loads every record in `edges.json` as
  *two* directed `Edge` objects (`from->to` and `to->from`) with identical
  distance/time. This is deliberately different from the one-way express
  line built separately in Round 2 (T2.3), which stays directed. See
  "Design Decisions" below for the reasoning.

### Data Input — everything is JSON

All input data (stations, edges, express-line graph, edge capacities, passengers,
trains) is stored as JSON in `data/` and parsed with the header-only
**nlohmann/json** library via `IO/JsonLoader.hpp`. No CSV is used anywhere in this
project, to keep a single consistent loading path (`FileLoader` interface →
`JsonLoader` implementation).

`stations.json`:
```json
{
  "stations": [
    { "id": 0, "name": "Qaleh Kamkar Station", "lat": 34.6499, "lng": 50.8764 },
    { "id": 1, "name": "Keshavarz Square Station", "lat": 34.6421, "lng": 50.8798 }
  ]
}
```
(`lat`/`lng` are real-world coordinates for Qom stations — needed for the Round 5
`AStar` heuristic. If left out for a station, that station simply can't be used with
`AStar` until filled in.)

`edges.json`:
```json
{
  "edges": [
    { "from": 0, "to": 1, "distance_km": 2.5, "time_min": 5 }
  ]
}
```

Later rounds reuse this loading path with their own files:
- `express_graph.json` — one-way express edges for the DAG shortest path (T2.3).
- `capacities.json` — per-edge passenger capacity for max-flow (T4.2).
- `incentives.json` — one-directional bonus weights for negative-cycle detection (T2.4).
- `trains.json` — train arrival/departure windows for platform allocation (T3.1) and
  dispatch priority (T3.2).
- `passengers.json` — sample passenger visit data for the statistics demo (T3.3).

Because JSON supports nesting, all of these can evolve independently without
breaking `stations.json`/`edges.json`.

---

## 5. Roadmap: Rounds ↔ Files ↔ Status

### Round 1 — Initial Admission (15 points)
| Code | Task | File(s) | Status |
|---|---|---|---|
| T1.1 | Graph modeling | `Core/*`, `IO/JsonLoader` | ☐ |
| T1.2 | Reachability (BFS/DFS) | `Algorithms/IReachability.hpp`, `BFS.hpp`, `DFS.hpp` | ☐ |
| T1.3 | Shortest-path engine (Dijkstra) | `Algorithms/IShortestPath.hpp`, `Dijkstra.hpp` | ☐ |
| T1.4 | Complexity analysis | Technical report | ☐ |

### Round 2 — Infrastructure Design (20 points)
| Code | Task | File(s) | Status |
|---|---|---|---|
| T2.1 | MST (compare Kruskal/Prim) | `Algorithms/IMst.hpp`, `Kruskal.hpp`, `Prim.hpp`, `Utils/Timer.hpp` | ☐ |
| T2.2 | Kruskal + Union-Find | `Structures/UnionFind.hpp`, `Algorithms/Kruskal.hpp` | ☐ |
| T2.3 | Express line (DAG shortest path) | `Algorithms/DAGShortestPath.hpp`, `data/express_graph.json` | ☐ |
| T2.4 | Negative-cycle detection (Bellman-Ford) | `Algorithms/BellmanFord.hpp` | ☐ |

### Round 3 — Daily Metro Operations (30 points)
| Code | Task | File(s) | Status |
|---|---|---|---|
| T3.1 | Shared-platform allocation (Interval Scheduling) | `Simulation/PlatformAllocator.hpp` | ☐ |
| T3.2 | Dispatch queue (Priority Queue) | `Simulation/DispatchQueueManager.hpp`, `Structures/PriorityQueue.hpp` | ☐ |
| T3.3 | Operational data analytics | `Simulation/Statistics.hpp` | ☐ |
| T3.4 | Passenger arrival simulation | `Simulation/PassengerSimulator.hpp`, `data/passengers.json` | ☐ |

### Round 4 — Network Performance Analysis (25 + 5 bonus)
| Code | Task | File(s) | Status |
|---|---|---|---|
| T4.1 | All-pairs shortest path (Floyd-Warshall) | `Algorithms/FloydWarshall.hpp`, `Services/AnalysisService.hpp` | ☐ |
| T4.2 | Capacity analysis (Max-Flow) | `Algorithms/IMaxFlow.hpp`, `FordFulkerson.hpp`, `EdmondsKarp.hpp`, `data/capacities.json` | ☐ |
| T4.3 | Critical stations (Articulation Points/Bridges) | `Algorithms/ArticulationPoints.hpp` | ☐ |
| T4.4 | Emergency team placement (bonus, NP-hard + approximation) | `Services/NetworkService.hpp` | ☐ |
| T4.6 | Typo-tolerant station name search (Levenshtein) | `Algorithms/Levenshtein.hpp`, `Services/SearchService.hpp` | ☐ |

### Round 5 — Innovation (bonus, 20 points)
| Track | Decision | Status |
|---|---|---|
| A | **AStar** — informed search using an admissible heuristic (straight-line distance between station coordinates), compared against `Dijkstra` on the Qom graph by number of expanded nodes | ☐ |

### Report (10 points)
| Item | Status |
|---|---|
| Final technical report in `docs/report.md` / `docs/report.pdf` | ☐ |

---

## 6. Design Decisions

1. **Reporting "no path found" / "negative cycle exists" (RESOLVED):** every
   algorithm result struct (`ReachabilityResult`, `PathResult`, ...) carries
   explicit `bool` fields (`reachable`, `hasNegativeCycle`) instead of using
   exceptions or `std::optional`. Exceptions are reserved for truly
   exceptional failures (a missing file, malformed JSON); "no path between
   these two stations" is a normal, expected outcome of a graph query, not
   an error. `IShortestPath::run()` is shared by Dijkstra, BellmanFord,
   DagShortestPath, and AStar; FloydWarshall (T4.1) does not implement it,
   since its all-pairs precompute model doesn't fit a single from/to query.
2. **Floyd-Warshall caching (RESOLVED): built once, queried many times.**
   `Algorithms::FloydWarshall` has an explicit `build()`/`query()` split
   instead of implementing `IShortestPath` -- `AnalysisService` calls
   `build()` once (at startup or on first use) and reuses the same
   instance for every subsequent `query()`, which runs in O(1) plus
   O(path length) to walk the cached `nextHop` matrix.
3. **Graph directedness (RESOLVED): the main graph is UNDIRECTED.** T2.3
   explicitly says the express line is a "one-way" edge set forming a DAG --
   that emphasis on one-way-ness only makes sense if the base graph is
   normally traversable in both directions. `JsonLoader` therefore loads
   every `edges.json` record as two directed `Edge`s.
4. **T2.4 negative-cycle triviality (RESOLVED): incentive weights are
   one-directional.** Because the graph is undirected, giving both
   directions of a station pair the same negative `bonusWeight` would make
   every such pair a trivial negative cycle (there and back = 2 * bonus <
   0), making detection meaningless. `setBonusWeight()` is therefore only
   ever called on ONE of the two directed `Edge` objects for a given pair
   -- modeling a one-way promotional discount (e.g. "discount for riding
   toward Pardisan," not the reverse). `BellmanFord` (T2.4) is the only
   algorithm that reads `Edge::effectiveWeight()`; everything else reads
   `Edge::weight()`, which never includes the bonus.
5. **`capacities.json` schema (RESOLVED, T4.2):** a top-level `capacities`
   array of `{ "from": int, "to": int, "capacity": double }` records, each
   setting ONE directed edge's capacity via `Graph::setEdgeCapacity()`
   (called from `JsonLoader::loadCapacities()`, itself part of the
   `FileLoader` interface now). Capacity is independent per direction
   (same convention as `bonusWeight`), so both directions of a physical
   connection must be listed separately if both should be limited. Edges
   not mentioned keep the default `Edge::capacity() == Utils::kInfinity`
   (unconstrained). A reference to a nonexistent edge is logged as a
   warning and skipped, not treated as a fatal error, since a stale
   capacities.json entry shouldn't take down routing/MST/search, which
   never touch capacity at all. `MetroSystem`'s constructor takes an
   optional `capacitiesFilePath` (empty = skip loading, T4.2's max-flow
   then simply runs unconstrained).
6. **`trains.json` / `passengers.json` schemas (RESOLVED):** both are
   simple flat arrays (`trains`, `visits`) loaded by dedicated
   `JsonLoader` methods (`loadTrains`, `loadVisits`) that are **not**
   part of the `FileLoader` interface, since neither has anything to do
   with building a `Graph` — see Section 6.

---

## 7. Build & Run Instructions

```bash
mkdir build && cd build
cmake ..
make
./QomMetro
```

Requirements: a C++17-capable compiler, CMake 3.15+, and an internet
connection the first time `cmake ..` runs (to fetch `nlohmann/json`;
CMake caches it under `build/_deps` afterward, so subsequent builds
don't need network access).

The `nlohmann/json` library is fetched via CMake's `FetchContent` at
configure time — it is never vendored as a file inside this repository.

---

## 8. Testing Strategy

This delivery does not include an automated `tests/` suite. Correctness
was instead verified by running every implemented capability through
the `cli/main.cpp` menu (17 options covering every task from T1.2
through Round 5) and cross-checking results where two algorithms solve
the same problem — e.g. Kruskal and Prim must report the identical MST
weight, Ford-Fulkerson and Edmonds-Karp must report the identical
max-flow value, and Dijkstra and A* must report the identical shortest-
path cost. The actual output of these runs is recorded in
`docs/report.md`, Section 12.

---

## 9. Development Workflow

1. Build the graph core (T1.1) → stabilize `IGraph`.
2. Implement Round 1 algorithms on top of the core.
3. Every subsequent round only adds new classes/files; existing files are not modified
   except for bug fixes.
4. `MetroSystem` (the Facade) delegates to `Services/*`, which are updated after each
   round to expose new capabilities — `MetroSystem` itself stays thin.
5. Before final submission: run the full project on a different machine + finish
   `docs/report.md`.

---

*This document will be updated as the project progresses — especially the "Status"
columns and the "Design Decisions" section.*
