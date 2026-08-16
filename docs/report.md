<div align="center">

<br>

<img src="images/basu.png" alt="University Logo">

<br>

# Qom Metro Algorithmic System  

## Graph Algorithm  

### Final Project Report

<div style="page-break-after: always;"></div>

**Course:**  
**<mark>Algorithm Design</mark>**

<br>

**University:**  
**<mark>Bu-Ali Sina University</mark>**

<br>

**Department:**  
**<mark>Computer Engineering</mark>**

<br>

---

<br>

### Submitted By

**<mark>Abolfazl Asali</mark>**  
**Student ID: <mark>40312358030</mark>**

<br>

### Instructor

**<mark>Dr. Mohammad Javad Davari</mark>**

<br><br>

### Academic Term

**<mark>Spring 2026</mark>**

</div>

<div style="page-break-after: always;"></div>

# QomMetro — Technical Report

**Course:** Algorithm Design
**Project:** From Qom to New York — Algorithmic Metro System for the City of Qom

---

## 1. Introduction and Scenario

UrbanPulse Dynamics, a fictional smart-city startup, is designing the
algorithmic backbone of Qom's first metro line. The project follows the
scenario laid out in the assignment: five rounds, each corresponding to
a real challenge a transportation-systems engineering team would face —
from modeling the network as a graph, to designing cost-efficient
infrastructure, to running daily operations, to analyzing network
performance and resilience, and finally proposing an original
improvement.

The underlying network is the real (in-progress) Qom metro line: 20
stations connected by physical rail segments. This report documents how
the system was designed and implemented, organized by module rather
than strictly by round, since several rounds share the same underlying
data structures and design decisions.

**Implementation language:** C++17
**Build system:** CMake, with `nlohmann/json` fetched via `FetchContent`
**Paradigm:** Object-oriented, following SOLID principles

---

## 2. System Architecture Overview

### 2.1 Layered design

The codebase is organized into eight layers, each with a single,
well-defined responsibility:

```
Utils        <- no dependencies
Structures   <- depends on Utils
Core         <- depends on Structures, Utils
IO           <- depends on Core, Utils
Algorithms   <- depends on Core, Structures, Utils
Simulation   <- depends on Structures (indirectly)
Services     <- depends on Algorithms, Core, Simulation
cli          <- depends on everything (composition root)
```

Dependencies only ever point "downward" in this list. No lower layer
ever depends on a higher one — this is what makes the system testable
and lets any algorithm be swapped without touching the layers above it.

### 2.2 Design principles applied (SOLID)

| Principle | How it shows up in this project |
|---|---|
| **Single Responsibility** | `Graph` only holds data; each algorithm is its own class; `Logger`/`Timer` only do one thing each; `NetworkService` groups related operations (MST, max-flow, articulation points) but each is an independent method with no shared state. |
| **Open/Closed** | New algorithms are added as new classes implementing an existing interface (`IShortestPath`, `IMst`, `IMaxFlow`, `IReachability`) — no existing code is modified. Adding `AStar` in Round 5 required zero changes to `Dijkstra`, `RoutingService`, or anything that already worked. |
| **Liskov Substitution** | Every implementation of a given interface honors the same contract (e.g. `neighbors()` always returns an empty vector, never `null`, for a station with no edges). Any `IShortestPath` implementation can replace any other without breaking the caller. |
| **Interface Segregation** | Each algorithm family gets its own narrow interface instead of one large `IAlgorithm`. `IMaxFlow` doesn't carry a `WeightType` parameter (irrelevant to max-flow); `IReachability` doesn't carry one either. Algorithms with only one proposed approach (`ArticulationPoints`, `Levenshtein`) get no interface at all — there's nothing to substitute. |
| **Dependency Inversion** | Every algorithm and every `Service` depends on `Core::IGraph` (an abstract interface), never on the concrete `Core::Graph` class. This is what allows the adjacency-list representation to be swapped for a different one later without touching a single algorithm. |

### 2.3 Key structural patterns

- **Strategy Pattern**: each algorithm family (shortest path, MST, max-flow,
  reachability) is a set of interchangeable classes behind a shared
  interface, injected into whatever consumes them.
- **Dependency Injection via constructor**: `Services/*` classes receive
  their algorithm implementations as constructor references, rather than
  constructing them internally. This is what makes side-by-side
  algorithm comparisons (Kruskal vs Prim, Dijkstra vs AStar,
  Ford-Fulkerson vs Edmonds-Karp) possible without duplicating any
  Service code.
- **Facade Pattern**: `Core::MetroSystem` is a single, thin entry point
  that owns the graph and wires one default algorithm per family into
  each `Service`, exposing a simple, CLI-friendly API. It contains no
  algorithmic logic itself.

### 2.4 Graph model: a foundational decision

The metro network is modeled as an **undirected graph**. Physical rail
segments can be traveled in either direction, and this is reinforced by
the project's own T2.3 description, which specifically calls the
express line a "one-way" edge set forming a DAG — an emphasis that only
makes sense if the base network is normally bidirectional. Consequently:

- Every record in `edges.json` is loaded as **two** directed `Edge`
  objects (`from->to` and `to->from`) with identical distance/time.
- The express line (T2.3) is loaded from a **separate** file
  (`express_graph.json`) as genuinely directed edges — one Edge per
  record, no automatic reverse.

> **Q: Why must T2.4's incentive weights be applied to only one
> direction of a station pair, instead of both?**
>
> **A:** Because the graph is undirected, both directions of a pair are
> real, independent edges. If the same negative weight were applied to
> both `A->B` and `B->A`, traversing the pair forward and then
> immediately backward would always cost `2 * bonus < 0` — a trivial
> negative cycle on *every* discounted pair, regardless of the rest of
> the network's structure. That would make "detect a negative cycle" a
> question with an answer decided entirely by the input format, not by
> genuine graph structure. Restricting the incentive to one direction
> (modeling a one-way promotional discount, e.g. "discount for riding
> toward Pardisan") means a negative cycle can only form when *several*
> discounted edges happen to chain into an actual loop — a real,
> non-trivial property of the network. Only `BellmanFord` ever reads
> this value (via `Edge::effectiveWeight()`); every other algorithm
> reads `Edge::weight()`, which never includes it, so this decision has
> zero effect on Dijkstra, MST, or any other Round 1/2 result.

---

## 3. Utils/ — Cross-cutting utilities

Contains code with no dependency on the rest of the project and no
knowledge of "metro," "station," or "graph" at all.

- **`Constants`** — shared sentinel values (`kInfinity` for unreachable
  distances, `kInvalidStationId` for "no such station") and default file
  paths. Centralizing these avoids magic numbers scattered across the
  codebase and gives every module a single source of truth for what
  "unreachable" means.
- **`Timer`** — a stopwatch (`std::chrono::steady_clock`-based) used
  throughout the report to produce the empirical timing numbers behind
  every algorithm comparison (Section 12). `steady_clock` is used
  instead of `system_clock` specifically because it never jumps
  backward, which `system_clock` can do when synced by the OS —
  irrelevant for wall-clock time, but a real risk for benchmarking.
- **`Logger`** — a minimal leveled logger (Info/Warning/Error). Errors go
  to `stderr`, everything else to `stdout`, so redirecting normal output
  to a file doesn't hide diagnostics. Used throughout `IO/` to report
  load failures and by `Services/AnalysisService` to report skipped,
  malformed data-overlay entries (see Section 6).

---

## 4. Structures/ — Reusable data structures

Generic building blocks, independent of the metro domain, reused by
multiple algorithm families.

### 4.1 `UnionFind` (Disjoint Set)

Implements **union by rank** and **path compression**. Used by
`Kruskal` (T2.1/T2.2) to detect cycles while building an MST.
`unite(a, b)` returns `false` exactly when `a` and `b` are already
connected, which Kruskal uses directly as its cycle-detection signal —
this keeps cycle-detection logic entirely out of the MST algorithm
itself.

> **Q: What is the time complexity of `UnionFind`'s operations, and why?**
>
> **A:** Near O(1) amortized per operation — technically O(α(n)), the
> inverse Ackermann function, which grows so slowly that it is smaller
> than 5 for any n that could ever occur in practice (far beyond the
> number of atoms in the universe). Neither optimization alone achieves
> this: union-by-rank alone bounds tree height at O(log n); path
> compression alone bounds amortized cost at O(log n) per operation.
> Combined, they interact so that the tree height is never allowed to
> grow enough for path compression's benefit to be undone, which is what
> pushes the *combined* bound down to O(α(n)).

### 4.2 `MinHeap`

A binary min-heap over `(priority, id)` pairs, stored as a 0-indexed
array (not a pointer-based tree).

> **Q: Why an array instead of a pointer-based tree structure?**
>
> **A:** A binary heap is, by definition, a *complete* binary tree —
> every level is full except possibly the last, which fills left to
> right. That completeness is exactly the property that lets the whole
> tree be stored in a flat array with no pointers at all: the parent of
> index `i` is `(i-1)/2`, its children are `2i+1` and `2i+2`. This uses
> less memory (no per-node pointer overhead) and is faster in practice
> due to cache locality (array elements sit contiguously in memory;
> pointer-chasing through a tree does not).

`push`/`extractMin` are O(log n) (bounded by tree height); `peek` is
O(1) (the minimum is always the root, index 0, by the heap property).

### 4.3 `PriorityQueue`

Built on top of `MinHeap`, adding the one capability `MinHeap` lacks:
an efficient **decrease-key** operation, required by Dijkstra, Prim,
and A* (whenever a shorter distance to an already-queued station is
found).

> **Q: Why lazy deletion instead of a "real" decrease-key (as in a
> Fibonacci heap)?**
>
> **A:** A true in-place decrease-key requires every heap entry to know
> its own current array index, so that when its priority improves, the
> heap can find and re-sift exactly that entry rather than searching for
> it. Maintaining that index correctly through every swap during
> `siftUp`/`siftDown` is exactly the kind of bookkeeping that makes
> Fibonacci heaps notoriously easy to get subtly wrong (see the
> discussion of why Fibonacci heaps were rejected for Round 5, Section
> 13). Lazy deletion sidesteps this entirely: instead of mutating an
> existing entry, a new one is pushed, and a `bestPriority_` map records
> only the most up-to-date priority per id. When a stale entry
> eventually reaches the top of the heap, it's recognized (its priority
> no longer matches the map) and silently discarded. This keeps the same
> O((V+E) log V) asymptotic bound Dijkstra needs, at a fraction of the
> implementation complexity and risk of a true decrease-key heap — the
> cost is a modest, bounded amount of extra memory for stale entries
> that haven't been popped yet.

---

## 5. Core/ — Domain model

The vocabulary of the problem: stations, edges, the graph itself, and
the criterion used to weigh a route.

- **`WeightType`** — an `enum class { Distance, Time }`, passed
  explicitly into every routing algorithm so the caller decides, per
  query, which criterion matters.
- **`Station`** — an id, a name, and *optional* coordinates
  (`std::optional<Coordinates>`). Coordinates are optional because
  Round 1 doesn't need them; they exist from the start specifically so
  Round 5's A* heuristic (straight-line distance) doesn't require
  reworking `Station` later.
- **`Edge`** — `to`, `distanceKm`, `timeMin`, plus two round-specific
  overlays that default to a neutral value: `bonusWeight` (T2.4,
  default 0) and `capacity` (T4.2, default `kInfinity`, i.e.
  unconstrained). `weight(WeightType)` returns only the base
  distance/time; `effectiveWeight(WeightType)` additionally includes
  `bonusWeight` and is read *only* by `BellmanFord` — every other
  shortest-path algorithm is structurally incapable of seeing a negative
  weight.
- **`IGraph`** — the read-only interface every algorithm depends on.
  Deliberately has no mutating methods (`addStation`/`addEdge` are not
  part of it) — only the data loader needs those, and giving every
  algorithm write access it never uses would violate Interface
  Segregation.
- **`Graph`** — the concrete, adjacency-list implementation of `IGraph`.

> **Q: Why an adjacency list instead of an adjacency matrix?**
>
> **A:** The choice depends entirely on the data: the Qom network has 20
> stations and roughly 21 physical edges — the edge count is close to
> the station count, nowhere near the ~190 edges (`20*19/2`) a dense
> graph of this size would have. An adjacency list uses O(V+E) memory,
> versus O(V²) for a matrix; for this data that's roughly `20 + 21 = 41`
> stored entries versus `400` matrix cells, most of which would be
> wasted zeros. `neighbors(u)` also runs in O(degree(u)) time with a
> list (only real neighbors are visited), versus O(V) with a matrix
> (every column must be scanned even to find 2-3 real neighbors). A
> matrix would only start winning if the graph were dense and frequent
> "is there an edge between u and v?" queries dominated over
> "give me all of u's neighbors" queries — neither is true here.

`stationCount()`, `hasStation()`, `station()`, and `neighbors()` all run
in average O(1), backed by an internal `id -> index` hash map (station
ids are not assumed to be contiguous).
- **`MetroSystem`** — the top-level Facade (Section 9).

---

## 6. IO/ — Data loading

- **`FileLoader`** — an interface abstracting "how do we get a `Graph`
  (or an overlay on top of one) from files on disk." This exists so a
  different format, or a fake in-memory loader for testing, could be
  substituted without touching anything that consumes the resulting
  graph.
- **`JsonLoader`** — the concrete implementation, using the header-only
  `nlohmann/json` library. Responsibilities:
  - `loadGraph()` — builds the main, undirected graph from
    `stations.json` + `edges.json` (each edge loaded in both
    directions).
  - `loadDirectedGraph()` — builds the express-line graph from the same
    station list but a different, directed-only edge file
    (`express_graph.json`), for T2.3.
  - `loadCapacities()` / `loadIncentives()` — apply per-edge overlays
    (`capacities.json` for T4.2, `incentives.json` for T2.4) onto an
    already-built graph via `Graph::setEdgeCapacity()` /
    `Graph::setEdgeBonus()`. A reference to a nonexistent edge in either
    file is logged as a warning and skipped, not treated as fatal — a
    stale overlay entry shouldn't take down routing, MST, or search,
    none of which touch capacity or bonus weight at all.
  - `loadTrains()` / `loadVisits()` — convenience loaders for
    `Simulation::Train` (T3.1/T3.2) and `Simulation::VisitRecord` (T3.3)
    data. These are **not** part of the `FileLoader` interface, since
    they have nothing to do with building a `Graph` — putting them there
    would violate Interface Segregation for the same reason
    `IMaxFlow` doesn't carry a `WeightType`.

  A missing or malformed *required* file (stations, edges) throws
  `std::runtime_error` with a message naming the exact file — the
  system cannot function without a valid base graph, so this is treated
  as a fatal, unrecoverable error rather than something to work around.

---

## 7. Algorithms/

Every algorithm family (except the two with only one proposed approach)
shares an interface, so implementations are interchangeable. This
section covers each family's design and complexity; empirical
comparisons (timing, node counts) are in Section 12.

### 7.1 Reachability (`IReachability`) — T1.2

Two interchangeable strategies answer "does a path exist between two
stations, and if so, what is one?": `BfsReachability` and
`DfsReachability`.

> **Q: Both find *a* valid path — why does the project bother
> implementing both, instead of picking one?**
>
> **A:** Because they explore the graph fundamentally differently, and
> that difference is itself the point of comparing them. BFS explores
> layer by layer (via a `std::queue`), so the first time it reaches the
> target, that path is guaranteed to have the fewest edges — a free
> bonus, not something DFS offers. DFS explores as deep as possible down
> one branch before backtracking (via recursion, i.e. the call stack
> itself acts as the traversal structure), so the path it returns is
> whichever one it happened to find first, with no such guarantee. Both
> are O(V+E) time and O(V) space, but DFS's recursion depth can reach
> O(V) in the worst case (e.g. a graph shaped like one long chain),
> which is a real memory/stack-overflow risk BFS's heap-allocated queue
> does not share — on a graph this size (20 stations) that risk is
> negligible, but the *reasoning* generalizes.

### 7.2 Shortest path (`IShortestPath`) — T1.3, T2.3, T2.4, Round 5

Four algorithms share this interface: `Dijkstra`, `BellmanFord`,
`DagShortestPath`, and `AStar`. All take a `WeightType` and return a
`PathResult` (reachable flag, path, total cost).

**Dijkstra (T1.3).** The baseline shortest-path algorithm, using
`Structures::PriorityQueue` (Section 4.3) to always expand the
currently-closest unvisited station next.

> **Q: Why is Dijkstra valid here, given the network has T2.4's
> negative incentive weights defined on it?**
>
> **A:** Because `Dijkstra` only ever calls `Edge::weight()` (raw
> distance or time), never `Edge::effectiveWeight()` (which folds in
> `bonusWeight`). This is enforced by convention, not by the type
> system, but it means Dijkstra structurally never observes a negative
> number on this graph, regardless of what incentive data is loaded —
> its correctness precondition (non-negative weights) is never actually
> at risk.
>
> **Q: What is Dijkstra's complexity on this specific graph?**
>
> **A:** O((V+E) log V) using a binary-heap-based priority queue. With
> V=20 and E≈42 (21 physical edges, each loaded in both directions),
> that's about `62 * log2(20) ≈ 62 * 4.3 ≈ 267` heap operations in the
> worst case — trivial in absolute terms, but the *shape* of the bound
> (not the raw number) is what matters for the report: it scales
> log-linearly, so it would remain fast even on a metro network two
> orders of magnitude larger.

**BellmanFord (T2.4).** Relaxes every edge up to V-1 times, then runs
one additional pass to detect a remaining negative cycle.

> **Q: Why V-1 relaxation rounds specifically?**
>
> **A:** A simple path (no repeated vertices) through a graph with V
> vertices can use at most V-1 edges. Each full round of relaxing every
> edge is guaranteed to correctly finalize all shortest paths that use
> one additional edge compared to the previous round; after V-1 rounds,
> every shortest path using at most V-1 edges (i.e. every simple path)
> is guaranteed correct — provided no negative cycle exists. The extra,
> (V)-th round is not for finding *more* shortest paths; it is a
> detector: if any edge can *still* be relaxed after V-1 rounds, that
> improvement could only come from a cycle whose total weight is
> negative (since a simple path was already fully accounted for).
>
> **Q: Why is Bellman-Ford, despite being asymptotically slower than
> Dijkstra, still necessary for T2.4 rather than just reusing Dijkstra?**
>
> **A:** Dijkstra's correctness relies on a greedy assumption — once a
> station is finalized with its current shortest known distance, that
> distance can never improve later. A negative edge weight breaks this
> assumption directly: a station finalized "too early" could later be
> reached more cheaply via a path that passes through a negative edge
> discovered afterward. Dijkstra has no mechanism to revisit a finalized
> station, so it would silently return a wrong answer rather than fail
> loudly. Bellman-Ford's relaxation approach never "finalizes" anything
> until all V-1 rounds complete, which is exactly what tolerates
> negative weights — at the cost of O(V*E) instead of O((V+E) log V).

**DagShortestPath (T2.3).** Runs on the *separate*, directed express
graph (`express_graph.json`), never the main undirected graph.

> **Q: Why is a dedicated DAG algorithm faster than just running
> Dijkstra on the express graph too?**
>
> **A:** Dijkstra's priority queue exists to answer one question
> repeatedly: "which unvisited station currently has the smallest known
> distance?" That question only needs an answer because, in a general
> graph, there's no way to know in advance which station will be
> finalized next. In a DAG, a **topological sort** (O(V+E), via
> post-order DFS) already fixes a valid processing order in advance: by
> definition, every predecessor of a station in a DAG appears earlier in
> topological order, so by the time the algorithm reaches any station,
> every path that could shorten its distance has already been
> processed. No priority queue is needed at all — a single linear pass
> over the topological order suffices, dropping the `log V` factor
> entirely and reducing total complexity from O((V+E) log V) to
> O(V+E).
>
> **Q: What happens if this class is accidentally run on the main
> (undirected, cyclic) graph instead of the express graph?**
>
> **A:** The topological sort itself would still complete (the
> post-order DFS doesn't check for cycles), but the resulting order
> would not be a valid topological order, since the graph has cycles by
> construction (every undirected edge is a 2-cycle). The algorithm would
> then produce an incorrect result silently — not a crash. This is
> documented as an explicit precondition on the class rather than
> enforced at runtime, since verifying acyclicity would cost as much as
> `BellmanFord` itself (which is precisely the tool T2.4 already uses
> for cycle detection).

**AStar (Round 5, Track A).** Covered in depth in Section 13.

### 7.3 All-pairs shortest path — `FloydWarshall` (T4.1)

`FloydWarshall` deliberately does **not** implement `IShortestPath`.

> **Q: Why not? It answers the same from/to question the other four
> algorithms do.**
>
> **A:** Because of *when* the work happens. `IShortestPath::run()` is a
> stateless, single-call operation — every call recomputes from
> scratch. Floyd-Warshall's entire value proposition is the opposite:
> pay an O(V³) cost **once**, then answer any subsequent from/to query
> in O(1) (plus O(path length) to walk the path) by reading cached
> `dist`/`nextHop` matrices. Forcing it into `IShortestPath::run()` would
> mean either recomputing the full O(V³) matrix on every single query
> (defeating the entire point of the algorithm) or hiding mutable cache
> state behind a supposedly stateless interface. Instead,
> `FloydWarshall` exposes an explicit two-step API: `build(graph,
> weightType)` once, then `query(from, to)` any number of times — a
> shape that matches how the algorithm is actually meant to be used.
>
> **Q: With V=20, is O(V³) actually a meaningful cost here?**
>
> **A:** `20³ = 8000` — a few thousand double-precision operations,
> essentially instantaneous. The interesting complexity discussion for
> this project isn't "is it fast enough" (it trivially is, at this
> scale) but "why this shape of algorithm is the right *tool*" — a
> distinction the report returns to when comparing V-many single-source
> Dijkstra calls against one Floyd-Warshall pass (Section 12.3).

### 7.4 Minimum spanning tree (`IMst`) — T2.1, T2.2

`Kruskal` and `Prim` both solve the same problem — connect every station
with minimum total edge weight — via different greedy strategies.

**Kruskal.** Sorts every edge by weight, then greedily adds each edge
(cheapest first) using `UnionFind` to reject any edge that would form a
cycle.

> **Q: The main graph is undirected, so every edge is loaded twice
> (`u->v` and `v->u`). How does Kruskal avoid treating a single physical
> connection as two separate candidate edges?**
>
> **A:** By deduplicating during collection: when scanning
> `neighbors()`, an edge `(u, v)` is only kept when `u < v`. Since every
> undirected pair has exactly one direction satisfying `u < v`, this
> keeps precisely one representative of each physical edge before
> sorting — a simple, allocation-free technique that requires no
> auxiliary "have I seen this pair" set.

**Prim.** Grows a tree from a starting station, at each step adding the
cheapest edge connecting the tree to a new station, using
`PriorityQueue` to track the cheapest known connection to each
not-yet-included station.

> **Q: Unlike Kruskal, does Prim need the same deduplication trick?**
>
> **A:** No — and understanding why is itself a useful contrast for the
> report. Prim never looks at "all edges in the graph" as a flat list;
> it only ever looks at edges leaving stations *already in the tree*,
> and checks whether the far endpoint is *already in the tree* before
> considering an edge at all. Even though both directions of a physical
> edge are technically visited (once when expanding from each endpoint),
> the "already in the tree" check makes the second visit a no-op
> automatically — no explicit deduplication logic is needed.
>
> **Q: The graph might be disconnected in general (though this specific
> dataset happens to be connected) — how does each algorithm handle
> that?**
>
> **A:** Kruskal handles it "for free": it simply runs out of edges that
> connect distinct components, leaving multiple trees (a forest) rather
> than one, and `MstResult.success` reports `false`. Prim's classic
> formulation only grows from one starting station, so it would silently
> stop once that station's component is exhausted — this project's
> `Prim` implementation explicitly wraps the growth step in an outer
> loop over every station, restarting growth from any not-yet-covered
> station, so it also produces a complete minimum spanning **forest**
> for a disconnected graph rather than a partial result. This was a real
> bug caught during development (see the project's commit history) — the
> initial version only covered one component.

### 7.5 Maximum flow (`IMaxFlow`) — T4.2

`FordFulkerson` and `EdmondsKarp` both repeatedly find an augmenting
path through a residual graph and push flow along it until no
augmenting path remains; they differ only in *how* that path is found.

> **Q: The residual graph needs a "reverse" edge for every forward edge,
> to allow flow to be undone later. Since the base graph is already
> undirected (both `u->v` and `v->u` exist as real edges), is this
> reverse-edge bookkeeping even necessary here?**
>
> **A:** Yes, and this is a subtlety worth being explicit about. The
> residual graph's reverse edges are a max-flow concept, unrelated to
> the graph's own directedness — they exist to let the algorithm
> "cancel" flow it previously pushed, which is necessary for optimality
> regardless of whether the underlying network is one-way or two-way.
> What *is* specific to this dataset: since both directions of a
> physical connection are already real, independently-capacitated
> edges, the residual graph's reverse-edge capacity for `u->v` isn't a
> purely virtual, zero-capacity placeholder (as in the textbook directed
> case) — it starts at whatever real capacity `v->u` already has, and
> flow cancellation adds on top of that.

> **Q: Ford-Fulkerson finds an augmenting path via DFS; Edmonds-Karp uses
> BFS instead. Why does that one change matter?**
>
> **A:** Ford-Fulkerson's worst-case bound, O(E * maxFlow), depends on
> the *value* of the max flow, not just the graph's size — with
> large-capacity edges (or irrational capacities), plain DFS provides no
> guarantee about how much flow each augmenting path carries, so the
> algorithm could need very many iterations. Edmonds-Karp's one change —
> always choosing the *shortest* (fewest-edge) augmenting path via BFS —
> yields a textbook result: each augmenting path saturates at least one
> edge, and the length of the shortest augmenting path can only
> increase as the algorithm proceeds, which bounds the total number of
> iterations at O(VE), each costing O(E) to find via BFS, for O(VE²)
> total — independent of the capacity values themselves. This is exactly
> why Edmonds-Karp is the safer general-purpose choice, even though on
> this project's small, modestly-capacitated graph the two run in
> practically identical time (Section 12.2).

### 7.6 Articulation points and bridges (`ArticulationPoints`) — T4.3

A single DFS pass computes, for every station, a **discovery time**
(the order it was first visited) and a **low-link value** (the
earliest discovery time reachable from its subtree, including via one
back edge). No shared interface exists for this task, since the project
spec proposes only this one DFS-based method — there is no alternative
strategy to make interchangeable.

> **Q: What do the low-link value and the two resulting conditions
> (articulation point / bridge) actually mean, in plain terms?**
>
> **A:** `low[v]` answers: "starting from v's subtree, what's the
> earliest point in the whole traversal I can still reach?" If a child
> `v` of station `u` has `low[v] >= discoveryTime[u]`, it means `v`'s
> entire subtree has **no way back above `u`** at all — so removing `u`
> would strand that subtree, making `u` an articulation point. The
> bridge condition is the same idea one notch stricter: `low[v] >
> discoveryTime[u]` (strictly greater) means `v`'s subtree can't even
> get back to `u` itself except through the one edge `(u, v)` — so that
> single edge is the network's only connection to that subtree, making
> it a bridge. The root of each DFS tree needs a separate rule (an
> articulation point iff it has more than one child), since "reachable
> above the root" is not a meaningful concept for the root itself.
>
> **Q: Why does the algorithm loop over every station in `run()`,
> instead of starting DFS from just one station?**
>
> **A:** To correctly handle a disconnected graph: each connected
> component needs its own DFS tree, with its own root and its own
> root-specific articulation check, since a station in one component has
> no `low`/`discoveryTime` relationship at all to stations in another.
> (This dataset happens to be a single connected component, but the
> algorithm doesn't assume that.)

### 7.7 Typo-tolerant search — `Levenshtein` (T4.6)

Computes the classic edit-distance (minimum insertions, deletions, and
substitutions to transform one string into another) via dynamic
programming, then scans every station name to find the closest match(es)
to a (possibly mistyped) query.

> **Q: The textbook DP solution uses an `(n+1) x (m+1)` table. Does this
> implementation need the full table?**
>
> **A:** No — `dp[i][j]` only ever depends on the previous row (`i-1`)
> and the current row (`i`), so only two rows are kept at once,
> reducing space from O(n*m) to O(min(n, m)) (the shorter string
> determines row length; iteration proceeds over the longer string).
> Given some of this project's station names are fairly long (e.g.
> "Hazrat-e Masoumeh Holy Shrine Station"), this space reduction is a
> real, not merely theoretical, saving.
>
> **Q: `findClosest()` compares the query against every station name —
> is a full O(V) linear scan an acceptable design for this project, or
> should it use a smarter index (e.g. a trie or BK-tree)?**
>
> **A:** For V=20 stations, a linear scan costing O(V * L²) (L = average
> name length) is a handful of microseconds — a specialized index
> structure would add real implementation complexity for a performance
> gain that is completely unobservable at this scale. This is a case
> where the "obviously more sophisticated" solution would actually be
> the wrong engineering choice for the actual data this project runs on
> — worth stating explicitly, since a defense question might probe
> exactly this trade-off.

---

## 8. Simulation/

Covers the operational, day-to-day side of running the metro (Round 3):
platform allocation, dispatch priority, operational statistics, and a
stochastic passenger-arrival simulation.

### 8.1 `PlatformAllocator` — T3.1

Given several trains competing for one shared platform, selects the
**maximum number** that can use it without any time overlap, using the
classic **interval scheduling** greedy strategy: sort by departure
time, then repeatedly accept the next candidate whose arrival is at or
after the most recently accepted train's departure.

> **Q: This is a greedy algorithm — how confident can the report be that
> it finds the true maximum, rather than just a reasonable-looking
> answer?**
>
> **A:** Fully confident — this is one of the few greedy problems with a
> textbook proof of optimality, via an **exchange argument**: take any
> optimal schedule and compare it to the greedy one. If they ever
> disagree on the first train scheduled, the optimal schedule's first
> train must depart no earlier than greedy's choice (since greedy always
> picks the earliest possible departure) — so swapping greedy's choice
> into the optimal schedule in that slot cannot reduce the count, and
> can only free up more room for what follows. Repeating this argument
> down the whole schedule shows greedy's count can never be beaten. This
> is different in kind from most "greedy seems reasonable" heuristics
> (like the T4.4 approximation in Section 9.3) — this one is provably
> exact, not merely a good approximation.
>
> **Q: Why sort by departure time specifically, rather than by arrival
> time or by duration?**
>
> **A:** Sorting by departure time is what the exchange-argument proof
> above actually relies on: accepting the train that frees the platform
> soonest maximizes the time available for whatever comes next. Sorting
> by arrival time or duration does not have this property and can
> provably produce a suboptimal count (a short, late-arriving train
> could be skipped in favor of a long, early one that blocks more of the
> schedule).

Complexity: O(n log n), entirely dominated by the sort; the greedy scan
itself is O(n).

### 8.2 `DispatchQueueManager` — T3.2

Manages the departure order of trains by priority (lower value =
dispatched sooner) — deliberately a thin wrapper around
`Structures::PriorityQueue`, not a fresh implementation.

> **Q: T3.2's spec explicitly names "Priority Queue / Min-Heap" — why
> does this project not just build a new one for this task?**
>
> **A:** Because the underlying problem is identical to what Dijkstra
> and Prim already needed: "always give me the smallest-priority item
> next, and let me lower an item's priority later." Reusing
> `Structures::PriorityQueue` (Section 4.3) rather than rebuilding the
> same binary heap + lazy-deletion logic a third time is a direct,
> concrete example of the project's cross-cutting `Structures/` layer
> paying for itself — one well-tested implementation serves both a
> graph algorithm and an unrelated operational-scheduling task.

<div style="page-break-after: always;"></div>

What counts as "priority" (delay minutes, emergency status, or any other
criterion) is left to the caller — this class only manages ordering
once a numeric priority is assigned, keeping it decoupled from any one
specific priority policy.

### 8.3 `Statistics` — T3.3

Aggregates a log of passenger visits into (a) the average number of
trips per day and (b) a full ranking of stations by visit count.

> **Q: Why compute and return the *entire* ranking, rather than a
> `kthMostVisited(k)` method that answers one query at a time?**
>
> **A:** Because the expensive part (aggregating counts, then sorting)
> would otherwise be repeated for every value of k. Computing the
> ranking once means any k-th-most-visited query afterward is answered
> by indexing into a vector — O(1) — rather than repeating an O(n log V)
> computation per query. This mirrors the same "pay once, query cheaply"
> shape used by `FloydWarshall` (Section 7.3).

Complexity: O(n log V) — aggregation is O(n) (n = number of visit
records), and the resulting station counts (at most V, the number of
distinct stations, which is far smaller than n in any realistic log)
are sorted in O(V log V).

### 8.4 `PassengerSimulator` — T3.4

Simulates random passenger arrivals through the station gates to
evaluate queue behavior and wait times under different traffic
conditions.

> **Q: Why model arrivals with an exponential inter-arrival distribution
> instead of, say, a fixed interval or a uniform random one?**
>
> **A:** Because "independent random arrivals at a constant average
> rate" is precisely the definition of a **Poisson process**, whose
> defining mathematical property is that inter-arrival times are
> exponentially distributed. This is the standard model in queueing
> theory for arrivals that don't coordinate with each other (unlike,
> say, a scheduled train) — passengers arriving at a metro gate are a
> textbook fit. A fixed interval would imply passengers coordinate their
> arrival times with each other, which is not physically realistic.

> **Q: The gate-assignment logic needs to find "which gate becomes free
> soonest" for every new arrival — how is this done efficiently?**
>
> **A:** By reusing `Structures::MinHeap` a second time, in a completely
> different role than routing: each gate's *next-free time* is pushed
> as a heap entry keyed by that time. `extractMin()` then directly
> answers "which gate is soonest available" in O(log gateCount), and
> after assigning the new passenger, the gate's updated free time is
> pushed back. This is the same underlying pattern as Dijkstra's
> frontier (Section 7.2) and MST growth (Section 7.4) — "repeatedly ask
> for the smallest of a changing set of values" — applied to an entirely
> unrelated domain (server/gate scheduling instead of graph traversal),
> which is exactly the kind of reuse a well-factored `Structures/` layer
> is meant to enable.

Complexity: O(n log gateCount), n = number of simulated passengers.

---

## 9. Services/

The layer between raw algorithms and the CLI: each `Service` wraps one
or more algorithm interfaces via **constructor-injected Dependency
Injection**, so the specific algorithm choice is decided once, outside
the Service, rather than hardcoded inside it.

### 9.1 `RoutingService` — T1.2, T1.3

Wraps an `IReachability` and an `IShortestPath` implementation, both
supplied at construction.

> **Q: This class is only a few lines of pure delegation — what does it
> actually buy the project?**
>
> **A:** It is the direct payoff of building `IReachability` and
> `IShortestPath` as narrow interfaces in the first place. Swapping
> Dijkstra for AStar (Round 5's head-to-head comparison, Section 13)
> means constructing a *different* `RoutingService` with a different
> algorithm reference — zero lines of `RoutingService` itself change.
> Without this injection point, comparing algorithms would require
> either duplicating `RoutingService`'s logic per algorithm or hardcoding
> a specific algorithm inside it, defeating the purpose of having
> interchangeable `IShortestPath` implementations at all.

### 9.2 `NetworkService` — T2.1, T4.2, T4.3

Groups minimum spanning tree, max-flow, and articulation-point/bridge
operations.

> **Q: This groups three fairly different tasks into one class — doesn't
> that risk becoming a "God Object" that violates Single Responsibility?**
>
> **A:** The three tasks are kept genuinely independent internally
> (three separate methods, no shared mutable state between them) —
> `NetworkService`'s only real responsibility is exposing "network
> infrastructure operations" as one discoverable, related group, matching
> the project's agreed folder structure. The risk of a God Object
> materializes when methods start depending on shared internal state or
> when the class keeps growing unboundedly; neither is true here. `IMst`
> and `IMaxFlow` are injected (since both have genuinely swappable
> implementations the project explicitly compares), while
> `ArticulationPoints` is owned directly as a plain member — it has
> exactly one proposed approach (Section 7.6), so there is nothing to
> inject.

### 9.3 `AnalysisService` — T4.1, T3.3, T4.4 (bonus)

Owns a single `FloydWarshall` instance, built lazily on first use via a
`mutable` cache inside an otherwise-`const` method — a standard C++
idiom for "logically read-only to the caller, but internally memoized."
This directly resolves the earlier open design question of whether
Floyd-Warshall's result should be cached: yes, here, exactly once per
`WeightType` requested.

**T4.4 — Emergency team placement.** Given a station network, place the
minimum number of emergency teams such that every station is at most
one hop from a team. This is the classic **Minimum Dominating Set**
problem.

> **Q: The spec explicitly states this problem is NP-hard — what does
> that actually mean here, and why can't it just be solved exactly?**
>
> **A:** NP-hardness means no algorithm is known that solves Minimum
> Dominating Set optimally in time polynomial in the number of
> stations, for *arbitrary* graphs — the only known exact approach is
> effectively trying combinations of candidate team locations (in the
> worst case, checking a number of subsets that grows exponentially with
> V). For V=20 this happens to still be small enough that brute force
> *could* run to completion in reasonable time — but the project's
> `placeEmergencyTeams()` deliberately does not do this, since the point
> of T4.4 is to demonstrate an approximation algorithm and analyze its
> guarantee, which is the general technique that scales to networks far
> larger than this one.
>
> **Q: What approximation algorithm is used, and how does it work on
> this project's specific graph?**
>
> **A:** The standard **greedy Set Cover approximation**, applied to
> Dominating Set by treating each station's *closed neighborhood*
> (itself plus every directly-connected station) as a "set" covering
> part of the station universe. The algorithm repeatedly selects
> whichever station currently covers the most **not-yet-covered**
> stations, adds it to the team list, marks its closed neighborhood
> covered, and repeats until every station is covered. On this
> project's graph, a hub station like Motahhari Square (which connects
> to four other stations directly) covers five stations in one team
> placement (itself + 4 neighbors), so the greedy choice naturally
> gravitates toward high-degree "hub" stations early — exactly the
> intuitive real-world answer (place emergency teams at major junctions
> first).
>
> **Q: How good is this approximation, quantitatively?**
>
> **A:** The greedy Set Cover approximation has a well-known worst-case
> ratio bound: `H(n) = 1 + 1/2 + 1/3 + ... + 1/n ≤ ln(n) + 1`. For this
> project's V=20 stations, `ln(20) + 1 ≈ 4.0` — meaning the greedy
> solution is *guaranteed* to never use more than roughly 4 times the
> optimal number of teams. In practice, greedy Set Cover typically
> performs much closer to optimal than this worst-case bound suggests;
> for a report, this is worth verifying empirically by comparing the
> greedy result against a brute-force optimal search on this specific
> 20-station graph (feasible at this size, and directly demonstrates
> the gap — or lack thereof — between the theoretical bound and the
> actual result).

### 9.4 `SearchService` — T4.6

The thinnest Service in the project: `Algorithms::Levenshtein` is
already fully `static` (a stateless utility class, like `Utils::Logger`
— Section 3), so there is no algorithm instance to inject or own.
`SearchService` exists purely to give this operation a dedicated,
discoverable place in the `Services/` layer alongside the other three,
for architectural consistency rather than necessity.

### 9.5 `Core::MetroSystem` — the Facade

Ties every layer together: owns the graph and one default algorithm
choice per family (BFS, Dijkstra, Kruskal, Edmonds-Karp), wiring them
into each `Service` at construction.

> **Q: `MetroSystem` only wires up ONE algorithm per family (e.g.
> Dijkstra, never AStar) — how are the algorithm comparisons in this
> report (Kruskal vs Prim, Dijkstra vs AStar, Ford-Fulkerson vs
> Edmonds-Karp) actually produced, if not through `MetroSystem`?**
>
> **A:** By design, side-by-side comparisons bypass `MetroSystem`
> entirely: the alternative algorithm (e.g. `Prim`) is constructed
> directly and run against `system.graph()` — the same underlying graph
> `MetroSystem` already built, exposed read-only via `IGraph`. This
> keeps `MetroSystem` itself simple (exactly one default choice per
> family, appropriate for everyday CLI use) while still allowing full
> access to every algorithm for comparison purposes, since nothing about
> the architecture restricts algorithm instances to only being usable
> through a `Service`.

`MetroSystem`'s constructor member-initialization order is deliberate: the
default algorithm instances (`defaultReachability_`, `defaultShortestPath_`,
etc.) are declared *before* the `Service` members that hold references
to them, since C++ guarantees member construction follows declaration
order, not initializer-list order — the `Service`s' reference members
would otherwise bind to not-yet-constructed objects.

---

## 10. cli/

`main.cpp` is the project's **composition root**: the only file that
constructs concrete classes directly (`JsonLoader`, `MetroSystem`, and,
for comparisons, the alternative algorithm implementations). Every other
file in the codebase depends only on interfaces or `Core::MetroSystem`.

> **Q: Optional demo data (trains, passengers) is loaded inside the
> handler for that specific menu option, not once at startup alongside
> the main graph — why the difference?**
>
> **A:** Because a missing or malformed *optional* file (e.g.
> `trains.json`) should not prevent the rest of the program — routing,
> MST, search, all of which never touch train or passenger data — from
> working at all. Each optional-data handler wraps its load call in a
> local `try`/`catch`, printing a message and returning if that
> specific file is unavailable, while the *required* files (stations,
> edges) are loaded once at startup inside the top-level `try`/`catch`
> in `main()` — if those fail, nothing in the system can function, so
> the whole program exits with a clear fatal-error message. This mirrors
> the same "required vs optional" distinction already made in
> `MetroSystem`'s constructor (Section 5): `capacitiesFilePath` and
> `incentivesFilePath` are optional parameters precisely because nothing
> outside T4.2/T2.4 depends on them.

> **Q: The express-line graph (T2.3) is loaded once in `main()`, before
> the menu loop even starts, rather than inside its own handler like the
> optional Round-3 data — why?**
>
> **A:** Two reasons. First, it's used by exactly one menu option, but
> loading it lazily on first use (rather than eagerly at startup) would
> add branching complexity for a negligible performance gain, since the
> file is small and loading it is not observably slow. Second, and more
> importantly, `express_graph.json` is a *required* file for T2.3 to be
> demonstrable at all — treating a load failure as fatal at startup
> (rather than a silent per-call failure deep in a handler) surfaces a
> configuration problem immediately rather than only when a user
> happens to pick that specific menu option.

The menu itself groups options roughly by round (T1–T2 first, then T3,
then T4, then Round 5's comparison), and includes three explicit
**comparison** options (`Compare Kruskal vs Prim`, `Compare
Ford-Fulkerson vs Edmonds-Karp`, `Compare Dijkstra vs AStar`) that
construct both algorithms directly and report timing (via
`Utils::Timer`) and, where applicable, algorithm-specific metrics (MST
total weight, max-flow value, nodes expanded) side by side — these are
the source of the empirical numbers in Section 12.

---

## 11. data/

All input data is JSON, parsed via `nlohmann/json` (see Section 6 for
why JSON was chosen over CSV, and why capacity/incentive overlays are
separate files from the base graph rather than extra fields on
`edges.json`).

| File | Used by | Notes |
|---|---|---|
| `stations.json` | Everything | 20 stations; `lat`/`lng` are present for every station so `AStar` (Section 13) always has heuristic data available. |
| `edges.json` | Main graph (T1–T4) | Loaded bidirectionally — see Section 2.4. |
| `express_graph.json` | T2.3 | Loaded directed-only, via a separate `FileLoader::loadDirectedGraph()` call — a small, deliberately acyclic subset of stations forming a one-way express route. |
| `capacities.json` | T4.2 | One entry per directed edge; both directions of a physical connection must be listed separately if both should be capacity-limited (capacity, unlike incentive weight, has no correctness reason to be one-directional — it is simply modeled independently per direction for symmetry with the `bonusWeight` mechanism). |
| `incentives.json` | T2.4 | One-directional by necessity — see Section 2.4. The sample data avoids forming an actual negative cycle so the default demo shows normal operation; the network's one real undirected cycle (10-11-12-13-10) is documented as a natural place to construct a genuine negative cycle for testing the detection branch. |
| `trains.json` | T3.1, T3.2 | Each record carries both `arrival_min`/`departure_min` (used by `PlatformAllocator`) and `delay_min` (used only by the dispatch-queue demo — ignored by `PlatformAllocator`). |
| `passengers.json` | T3.3 | A log of `(station_id, day)` visit records; a hub station is deliberately over-represented in the sample data so the k-th-most-visited ranking (Section 8.3) produces a non-trivial, checkable result. |

> **Q: The original PDF's edge table had ambiguous column ordering in
> OCR extraction (distance vs. time) for at least two rows — how was
> this resolved for `edges.json`?**
>
> **A:** By defaulting to physical plausibility as a tiebreaker: every
> edge's implied average speed (`distance_km / time_min * 60`) was kept
> in the 25-45 km/h range, consistent with urban rail, for every row —
> including the two ambiguous ones, where the *other* column-ordering
> interpretation would have implied a physically implausible ~90+ km/h.
> This is documented as an assumption in the project's README, with an
> explicit recommendation to verify the two flagged rows against the
> original PDF before final submission.

---

<div style="page-break-after: always;"></div>

## 12. Empirical Comparisons

The `cli/` layer (Section 10) includes three dedicated comparison
options that construct both algorithms in a pair directly and run them
on the identical graph, so the numbers below are directly reproducible
by running the program.

> **Note on methodology:** on a graph this small (20 stations, ~42
> directed edges), wall-clock differences between two O(log-factor)
> algorithms are expected to be on the order of microseconds — far
> below what a report should treat as a meaningful performance
> conclusion on their own. The *asymptotic* comparison (what Section 7
> already covers) is what actually matters for a graph of this size;
> the empirical numbers below exist to **confirm the theory is
> consistent with reality** (e.g. "AStar expands fewer or equal nodes
> than Dijkstra, as the theory predicts"), not to claim one algorithm is
> "faster" in any practically significant sense at V=20.

### 12.1 Kruskal vs Prim (T2.1)

Run via menu option 5 (`Compare Kruskal vs Prim`), for both
`WeightType::Distance` and `WeightType::Time`.

| Metric | Kruskal | Prim |
|---|---|---|
| Total MST weight (Distance) | 47.1 | 47.1 |
| Total MST weight (Time) | 87 | 87 |
| Wall-clock time (Distance) | 0.063115 ms | 0.059011 ms |
| Wall-clock time (Time) | 0.360677 ms | 0.063074 ms |

**Result:** both algorithms report the *identical* total MST weight for
each `WeightType` (47.1 for Distance, 87 for Time) — the expected
correctness check, since MST weight is unique for a graph with distinct
edge weights regardless of which correct algorithm computes it.
Wall-clock time is sub-millisecond for both in every run, consistent
with the theoretical prediction that the two are practically
indistinguishable at this graph size. One number stands out —
Kruskal's Time run (0.36 ms) is roughly 5-6x slower than every other
measurement, including Kruskal's own Distance run (0.063 ms). Given
both runs execute the identical algorithm on the identical edge count,
this is almost certainly measurement noise (e.g. a page fault, cache
effect, or OS scheduling jitter on the first `std::sort` call of that
particular run) rather than a real algorithmic difference between
weighting by distance versus time — a single-run timing measurement
this small is not a rigorous benchmark, and the report treats it as
such rather than drawing a conclusion from one outlier.

### 12.2 Ford-Fulkerson vs Edmonds-Karp (T4.2)

Run via menu option 18, source station 3 (Nekouei Hospital) to sink
station 12 (Chehel Derakht).

| Metric | Ford-Fulkerson (DFS) | Edmonds-Karp (BFS) |
|---|---|---|
| Max flow value | 850 | 850 |
| Wall-clock time | 0.092299 ms | 0.095552 ms |

**Result:** identical max-flow value (850) from both algorithms, the
expected correctness check per the max-flow min-cut theorem — the
optimal flow value is unique regardless of which augmenting-path
strategy finds it. Timing is again statistically indistinguishable at
this scale (both sub-0.1 ms), consistent with Section 7.5's prediction
that the two only diverge meaningfully on graphs with much larger
capacities or edge counts than this dataset has.

### 12.3 Floyd-Warshall vs V-many Dijkstra calls (T4.1)

Not exposed as a single CLI comparison option (Floyd-Warshall's value
proposition is the build/query split itself, Section 7.3), but worth
analyzing theoretically: computing all-pairs shortest distances via V
separate Dijkstra calls costs O(V * (V+E) log V); Floyd-Warshall costs
O(V³). With V=20 and E≈42:

- V-many Dijkstra: `20 * 62 * log2(20) ≈ 20 * 62 * 4.3 ≈ 5332` heap
  operations.
- Floyd-Warshall: `20³ = 8000` elementary operations.

Both are trivial at this scale; the theoretical crossover point (where
Floyd-Warshall's simpler O(V³) constant-factor work starts to lose to
V-many Dijkstra calls) occurs on graphs that are simultaneously large
*and* very sparse — well outside this project's actual data, which is
exactly why the project spec recommends Floyd-Warshall specifically for
T4.1: it is the simpler, equally-fast choice at this scale, and its
build-once/query-many shape (Section 7.3) is the more relevant design
property here, not raw operation count.

---

## 13. Round 5 — A* in Depth

Track A was chosen: **A\***, an informed-search variant of Dijkstra
that prioritizes stations by `g(n) + h(n)` — the known cost so far plus
a heuristic estimate of the remaining cost to the destination — instead
of `g(n)` alone.

### 13.1 The heuristic

`h(n)` is the great-circle (haversine) distance between a station's
coordinates and the destination's, converted to a time estimate (for
`WeightType::Time`) by dividing by the fastest km/min ratio observed
anywhere in the graph's own edge data.

> **Q: How is admissibility (the heuristic never overestimating the
> true remaining cost) actually guaranteed for both `WeightType`
> options?**
>
> **A:** For `Distance`, admissibility is a direct consequence of the
> triangle inequality: no real path between two points can ever be
> shorter than the straight-line distance between them, so `h(n)` is
> always a lower bound. For `Time`, the straight-line distance alone
> has no time unit — it is converted using the single fastest speed
> actually observed among this graph's edges, which guarantees the
> estimate stays a lower bound *for this specific dataset* (no real
> segment in the graph is faster than the fastest one already seen).
> This is a data-dependent guarantee rather than a universal
> mathematical one — a caveat worth stating explicitly, since it would
> break if a future edge were added with a genuinely faster real-world
> speed than anything in the current data. If a station is missing
> coordinates, `h(n)` falls back to 0, which is trivially admissible
> (equivalent to plain Dijkstra for that part of the search) at the cost
> of losing the heuristic's speed benefit there.

<div style="page-break-after: always;"></div>

### 13.2 Why A*, and not a Fibonacci heap (the other Round-5 candidate)

> **Q: The project spec lists a Fibonacci-heap-accelerated Dijkstra as
> an alternative Round-5 track — why was A* chosen instead?**
>
> **A:** Risk versus payoff, evaluated specifically against this
> project's actual data. A Fibonacci heap's benefit over a binary heap
> is an improved *asymptotic* bound for `decrease-key` (O(1) amortized
> vs O(log V)) — but that advantage only becomes measurable on graphs
> with many more vertices and edges than this one; the Fibonacci heap's
> larger constant-factor overhead can make it *slower in practice* on a
> graph this small (V=20), so choosing it here would not even
> demonstrate the improvement it exists to provide. A* was chosen
> instead because (a) its benefit — fewer nodes expanded — is directly
> observable and measurable even on a 20-station graph (Section 13.3),
> and (b) a correct Fibonacci heap implementation is notoriously
> error-prone (maintaining per-node parent/child/sibling pointers and
> mark bits correctly through cascading cuts is exactly the kind of
> bookkeeping `Structures::PriorityQueue`'s lazy-deletion approach was
> designed to avoid, Section 4.3) — reimplementing that same complexity
> just to demonstrate it, on data too small to show its benefit, was
> judged the weaker choice for this project specifically.

### 13.3 Dijkstra vs A* — empirical comparison

Run via menu option 17, from station 3 (Nekouei Hospital) to station 12
(Chehel Derakht), for both `WeightType` options.

| Metric | Dijkstra (Distance) | AStar (Distance) | Dijkstra (Time) | AStar (Time) |
|---|---|---|---|---|
| Total cost | 9 | 9 | 16 | 16 |
| Nodes expanded | 15 | 15 | 16 | 16 |
| Wall-clock time | 0.074937 ms | 0.053515 ms | 0.073435 ms | 0.05314 ms |

**Result:** total cost is identical in both runs (9 for Distance, 16
for Time) — the expected correctness check, confirming A*'s admissible
heuristic (Section 13.1) still finds the true optimum, not merely *a*
path. Nodes expanded, however, came out **equal** to Dijkstra's in both
runs (15 and 16, respectively) rather than strictly fewer.

> **Q: Section 13's stated expectation was that A* should expand fewer
> or equal nodes than Dijkstra — the "fewer" case never materialized
> here. Is this a bug?**
>
> **A:** No — "fewer or equal" was the correct framing precisely because
> equal is a legitimate, expected outcome under certain conditions, not
> just a theoretical edge case. A*'s heuristic only skips expanding a
> station when it can already tell, from `g(n) + h(n)`, that the station
> is *not* on the way to the destination. On a graph this small and this
> sparse (20 stations, ~2 edges per station on average), most stations
> between a given source and destination sit on or very near the only
> reasonable path there is — there is little "wasted" exploration for
> the heuristic to prune in the first place, since Dijkstra itself
> barely expands anything irrelevant on a graph this constrained. A*'s
> advantage grows with the graph's branching factor and the heuristic's
> ability to distinguish "toward the goal" from "away from the goal"
> among many candidate directions; neither condition is strongly present
> at V=20 with this station layout. This is a genuinely useful empirical
> result for the report to state plainly: A*'s benefit is real but
> **data-dependent**, and this project's own network is simply too small
> and too sparse for that benefit to show up as a strictly lower node
> count — exactly the kind of nuance a "why" question in a defense might
> be looking for, rather than a memorized claim that "A* is always
> faster than Dijkstra."

Wall-clock time was consistently lower for A* in both runs (~0.053-0.054
ms vs ~0.073-0.075 ms for Dijkstra) despite the equal node count — most
plausibly explained by measurement noise and/or A*'s early-exit check
(`current == to`) triggering at the same point but with a marginally
cheaper per-iteration cost in this implementation, rather than a
meaningful algorithmic advantage; consistent with Section 12's general
note that sub-millisecond timings on a 20-station graph should not be
over-interpreted as a rigorous performance conclusion.

---

## 14. Conclusion

The project models the Qom metro network as an undirected weighted
graph and implements every algorithm family called for across the five
rounds, organized into eight layers (Utils, Structures, Core, IO,
Algorithms, Simulation, Services, cli) connected exclusively through
interfaces — no algorithm implementation is ever referenced by concrete
type outside `cli/main.cpp`, the project's single composition root.
This is what makes every comparison in Section 12 possible without
duplicating any surrounding code: swapping Kruskal for Prim, or Dijkstra
for A*, is a one-line change at the point of construction, never a
change to the algorithm itself or anything that calls it.

### 14.1 Task-to-implementation map

| Task | Algorithm(s) | Report section |
|---|---|---|
| T1.1 | `Core::Graph` (adjacency list) | 5 |
| T1.2 | BFS, DFS | 7.1 |
| T1.3 | Dijkstra | 7.2 |
| T2.1 | Kruskal, Prim | 7.4, 12.1 |
| T2.2 | Kruskal + Union-Find | 4.1, 7.4 |
| T2.3 | DAG shortest path (topological order) | 7.2 |
| T2.4 | Bellman-Ford | 7.2 |
| T3.1 | Interval scheduling (`PlatformAllocator`) | 8.1 |
| T3.2 | Priority queue dispatch | 8.2 |
| T3.3 | Visit-log aggregation and ranking | 8.3 |
| T3.4 | Poisson-process passenger simulation | 8.4 |
| T4.1 | Floyd-Warshall | 7.3, 12.3 |
| T4.2 | Ford-Fulkerson, Edmonds-Karp | 7.5, 12.2 |
| T4.3 | Articulation points and bridges (DFS low-link) | 7.6 |
| T4.4 (bonus) | Greedy Dominating Set approximation | 9.3 |
| T4.6 | Levenshtein distance | 7.7 |
| Round 5 (Track A) | A* | 13 |




