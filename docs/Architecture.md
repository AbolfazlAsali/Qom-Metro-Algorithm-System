# Architecture

> **Project:** From Qom to New York — Qom Metro Algorithmic System  
> **Language:** Modern C++ (C++17+)  
> **Architecture:** Object-Oriented, SOLID, Interface-Oriented Design  
> **Build System:** CMake

---

# Table of Contents

- Introduction
- Design Philosophy
- Architecture Goals
- Project Organization
- Folder Structure
- Layered Architecture
- Core Components
- Dependency Rules
- Design Principles
- Why This Architecture?
- What's Next?

---

# Introduction

This document describes the software architecture of the **Qom Metro Algorithmic System**.

While the main `README.md` introduces the project and explains its purpose, this document focuses on **how the software is designed**, **why specific architectural decisions were made**, and **how the project can grow over time without sacrificing maintainability**.

The primary objective of this project is not merely to implement graph algorithms, but to integrate them into a coherent, modular, and extensible software system. Every algorithm operates on the same shared graph infrastructure, ensuring consistency across all development rounds.

This architecture has been designed from the beginning with future expansion in mind. As new algorithms, simulations, and analysis tools are introduced in later rounds, the existing codebase should require little to no modification.

---

# Design Philosophy

This project follows a simple philosophy:

> **The graph stores data. Algorithms solve problems. Services coordinate behavior. MetroSystem orchestrates the application.**

Each component has a clearly defined responsibility.

Instead of placing every algorithm inside a single graph class, responsibilities are separated into multiple independent modules.

This separation results in code that is easier to understand, easier to test, and significantly easier to extend.

The architecture favors composition over tightly coupled implementations and relies heavily on interfaces to isolate high-level components from implementation details.

---

# Architecture Goals

The architecture was designed with the following goals in mind.

- Modular implementation
- Clear separation of responsibilities
- High readability
- Maintainability
- Extensibility
- Reusability
- Testability
- Low coupling
- High cohesion
- Compliance with SOLID principles

Every architectural decision made throughout the project supports one or more of these goals.

---

# Project Organization

The project is divided into multiple independent modules.

Each module focuses on one specific responsibility and communicates with other modules through well-defined interfaces.

```text
QomMetro/
│
├── include/
│
│   ├── Core/
│   │
│   ├── Algorithms/
│   │
│   ├── Structures/
│   │
│   ├── Simulation/
│   │
│   ├── Services/
│   │
│   ├── IO/
│   │
│   └── Utils/
│
├── src/
│
├── data/
│
├── docs/
│
├── tests/
│
├── output/
│
├── third_party/
│
└── CMakeLists.txt
```

The implementation files mirror the same organization inside the `src/` directory.

This one-to-one correspondence makes navigation straightforward and allows developers to quickly locate source files associated with a particular module.

---

# Folder Structure

## include/

The `include` directory contains every public header used by the application.

Headers are organized by responsibility rather than by algorithm difficulty or project round.

Separating declarations from implementations improves compilation times, encourages modularity, and keeps dependencies under control.

Typical contents include:

- Core classes
- Algorithm interfaces
- Concrete algorithm implementations
- Data structures
- Services
- Input/output utilities
- Simulation modules
- Utility classes

---

## src/

The `src` directory contains the implementation of every class declared in `include/`.

The internal organization mirrors the directory hierarchy found inside `include`.

For example,

```text
include/
    Algorithms/
        Dijkstra.hpp

↓

src/
    Algorithms/
        Dijkstra.cpp
```

Maintaining identical folder layouts greatly simplifies project navigation.

---

## data/

All runtime data is stored inside the `data` directory.

Typical files include:

```text
stations.json

edges.json

express_graph.json

capacities.json

passengers.json

trains.json
```

The application never hardcodes metro data inside the source code.

Instead, every graph instance is created by loading structured JSON files.

This approach keeps the implementation independent of specific datasets and allows future datasets to be introduced without recompilation.

---

## docs/

The `docs` directory contains all project documentation.

Typical contents include:

```text
docs/

README images

Architecture.md

Technical Report

UML Diagrams

Benchmark Results
```

Keeping documentation separate from source code makes the repository easier to navigate while preserving a clean project root.

---

## tests/

Every major subsystem has its own dedicated tests.

Instead of grouping tests by project round, tests are grouped by module.

Example:

```text
tests/

GraphTests.cpp

RoutingTests.cpp

MSTTests.cpp

FlowTests.cpp

SimulationTests.cpp
```

This organization ensures that future algorithms can reuse the same testing infrastructure.

---

## output/

The application stores generated files inside this directory.

Typical examples include:

- execution logs
- benchmark reports
- exported statistics
- generated simulation data

No generated files should be committed to version control unless explicitly required.

---

## third_party/

This directory contains external dependencies that are distributed with the project.

Currently, it contains the header-only version of **nlohmann/json**, allowing JSON parsing without additional installation steps.

Because the dependency is header-only, integration remains simple while keeping the project portable across operating systems.

---

# Layered Architecture

The project follows a layered architecture.

Instead of allowing every component to communicate directly with every other component, responsibilities are divided into distinct layers.

```text
Application Layer
│
├── MetroSystem
│
Service Layer
│
├── RoutingService
├── NetworkService
├── AnalysisService
└── SearchService
│
Algorithm Layer
│
├── Shortest Path
├── MST
├── Max Flow
├── Reachability
└── Search Algorithms
│
Core Layer
│
├── Graph
├── Station
└── Edge
│
Data Layer
│
└── JSON Files
```

Each layer communicates only with the layer directly below it.

This significantly reduces coupling and prevents unrelated modules from depending on one another.

---

# Why Layered Architecture?

A layered architecture provides several important advantages.

First, it isolates responsibilities.

For example, user interface code never needs to know how Dijkstra's algorithm is implemented.

Instead, it simply asks the corresponding service to compute a route.

Likewise, graph algorithms never interact with menus, console output, or JSON parsing.

Each layer focuses exclusively on its own task.

This design also makes future development significantly easier.

Adding a new shortest-path algorithm requires only implementing the appropriate interface and registering it inside the routing service.

No changes are required in the graph implementation or user interface.

---

# What's Next?

The following sections describe every major module of the project in detail.

The next part of this document covers:

- Core Components
- MetroSystem
- Graph
- Station
- Edge
- Interfaces
- Services
- Dependency Rules
- Object Responsibilities

These components form the foundation upon which every algorithm and simulation module is built.


# Core Components

The core of the project consists of a small number of fundamental classes.

Every other component in the system depends directly or indirectly on these classes.

The core is intentionally kept as small and stable as possible because nearly every algorithm in the project relies on it.

The following components form the backbone of the application:

```text
Core/

MetroSystem

Graph

Station

Edge

WeightType
```

Each of these classes has a clearly defined responsibility.

---

# MetroSystem

`MetroSystem` is the central controller of the entire application.

It acts as the **Facade** of the project, providing a single entry point for all user interactions.

Instead of allowing the user interface to communicate directly with algorithms, the application delegates every request to `MetroSystem`, which forwards the request to the appropriate service.

Typical responsibilities include:

- Initializing the application
- Loading metro data
- Creating the graph
- Constructing services
- Managing the application workflow
- Displaying the main menu
- Delegating user requests
- Coordinating communication between modules

The class intentionally contains very little algorithmic logic.

Its purpose is coordination rather than computation.

---

## Typical Workflow

```text
Application Starts
│
├── Create MetroSystem
│
├── Load JSON Files
│
├── Construct Graph
│
├── Initialize Services
│
├── Display Menu
│
└── Wait for User Commands
```

This design keeps the application organized and prevents the `main()` function from becoming excessively large.

---

# Graph

The `Graph` class is the most important data container in the project.

Its responsibility is **only** to store the metro network.

It does **not** implement graph algorithms.

Instead, algorithms receive a reference to the graph and perform their computations independently.

Typical responsibilities include:

- Store stations
- Store edges
- Build adjacency lists
- Insert new vertices
- Insert new edges
- Remove vertices
- Remove edges
- Return neighbors
- Query graph information

Because the graph only stores data, it remains stable throughout all project rounds.

---

## Graph Representation

The metro network is represented using an adjacency list.

Conceptually, the structure is similar to:

```text
Graph
│
├── Station A
│      ├── Edge
│      ├── Edge
│      └── Edge
│
├── Station B
│      ├── Edge
│      └── Edge
│
└── Station C
       ├── Edge
       └── Edge
```

This representation is well suited for sparse graphs such as transportation networks.

Compared to an adjacency matrix, it requires significantly less memory while maintaining efficient traversal performance.

---

# Station

A `Station` represents one metro station.

It contains only information directly related to that station.

Typical fields include:

```text
Station

id

name

latitude

longitude
```

Coordinates are included from the beginning because they will later support heuristic routing algorithms such as **A\***.

Keeping this information inside the station avoids modifying the data model in future development rounds.

---

# Edge

An `Edge` represents a connection between two stations.

Unlike a station, an edge describes the relationship between two vertices.

Typical fields include:

```text
Edge

destination

distance

travelTime

capacity

bonusWeight
```

Not every field is required in every project round.

Some values, such as passenger capacity, become relevant only during network analysis.

Designing the class with future expansion in mind minimizes future code changes.

---

# WeightType

Routing algorithms may optimize different criteria.

Instead of creating separate algorithms for every criterion, the project introduces an enumeration.

```text
enum class WeightType

Distance

Time
```

The caller selects the desired optimization criterion, while the routing algorithm remains unchanged.

This approach avoids code duplication and improves flexibility.

---

# Interfaces

The project relies heavily on interface-oriented programming.

Instead of coupling services directly to concrete implementations, they depend only on abstract interfaces.

Typical interfaces include:

```text
Interfaces/

IGraph

IReachability

IShortestPath

IMST

IMaxFlow
```

This design enables algorithms to be replaced or extended without affecting higher-level modules.

---

# IGraph

`IGraph` defines the contract for every graph implementation.

Algorithms never depend directly on `Graph`.

Instead, they communicate exclusively through this interface.

Typical operations include:

- Retrieve vertices
- Retrieve neighbors
- Retrieve edges
- Query graph size

If a different graph representation is introduced in the future, algorithms remain unchanged because the interface contract is preserved.

---

# IReachability

This interface represents graph traversal algorithms.

Current implementations include:

```text
IReachability
│
├── BFS
└── DFS
```

Every traversal algorithm exposes the same public interface, allowing them to be exchanged without modifying application code.

---

# IShortestPath

Shortest-path algorithms implement a common interface.

Current implementations include:

```text
IShortestPath
│
├── Dijkstra
├── BellmanFord
├── FloydWarshall
├── DAGShortestPath
└── AStar
```

Because every implementation follows the same contract, services can execute different algorithms transparently.

---

# IMST

Minimum spanning tree algorithms share another dedicated interface.

Current implementations include:

```text
IMST
│
├── Prim
└── Kruskal
```

Both algorithms solve the same problem while using different strategies.

The interface allows either implementation to be selected without changing client code.

---

# IMaxFlow

Maximum-flow algorithms also follow an interface-based design.

Current implementations include:

```text
IMaxFlow
│
├── FordFulkerson
└── EdmondsKarp
```

Future flow algorithms can be added simply by implementing the same interface.

---

# Why Interfaces?

Interfaces provide several important advantages.

- Reduce coupling
- Increase flexibility
- Simplify testing
- Encourage modular design
- Enable dependency injection
- Allow algorithms to evolve independently

Most importantly, they ensure that high-level modules depend on abstractions rather than concrete implementations.

---

# Services

Algorithms are never called directly by the user interface.

Instead, all communication passes through dedicated services.

The service layer acts as the bridge between the application and the algorithm implementations.

```text
MetroSystem
│
├── RoutingService
├── NetworkService
├── AnalysisService
└── SearchService
```

Each service owns a single functional responsibility.

---

# RoutingService

The RoutingService provides all routing-related functionality.

Responsibilities include:

- Reachability analysis
- Shortest-path computation
- Route selection
- Path reconstruction
- Algorithm dispatch

Supported algorithms include:

```text
RoutingService

BFS

DFS

Dijkstra

BellmanFord

FloydWarshall

DAGShortestPath

AStar
```

The user interface never communicates directly with these algorithms.

Instead, every request is routed through this service.

---

# NetworkService

NetworkService manages algorithms related to network optimization.

Responsibilities include:

- Minimum spanning trees
- Connectivity
- Maximum flow
- Future infrastructure analysis

Typical algorithms include:

```text
Prim

Kruskal

FordFulkerson

EdmondsKarp
```

---

# AnalysisService

AnalysisService focuses on analytical tasks.

Examples include:

- Network statistics
- All-pairs shortest paths
- Critical station analysis
- Performance metrics

Separating analytics from routing keeps both modules focused and maintainable.

---

# SearchService

SearchService is responsible for station name matching.

Current implementation:

```text
Levenshtein Distance
```

This enables typo-tolerant station searches without affecting routing or graph logic.

---

# What's Next?

The next section explains:

- Dependency Rules
- Data Flow
- SOLID Principles
- Design Patterns
- Object Collaboration
- Application Workflow

These concepts describe **how all components interact while remaining loosely coupled**.


# Dependency Rules

One of the primary goals of this architecture is to minimize coupling between modules.

To achieve this, the project follows a strict dependency hierarchy.

Higher-level components coordinate behavior, while lower-level components provide reusable functionality.

The dependency direction is intentionally one-way.

```text
Application
│
├── MetroSystem
│
├── Services
│
├── Interfaces
│
├── Algorithms
│
├── Core
│
└── Data
```

A lower layer must never depend on a higher layer.

For example:

- Algorithms must never access menus.
- Graph must never know which algorithm is executing.
- JSON loaders must never execute algorithms.
- Services must never manipulate graph internals directly.

This rule significantly reduces coupling and keeps every module reusable.

---

# Dependency Flow

The following illustrates how a typical request travels through the application.

```text
User
│
▼
Console Menu
│
▼
MetroSystem
│
▼
RoutingService
│
▼
IShortestPath
│
▼
Dijkstra
│
▼
IGraph
│
▼
Graph
│
▼
Result
```

Each component performs exactly one responsibility before passing control to the next layer.

No shortcuts are allowed.

---

# Data Flow

The movement of data through the application follows a predictable pipeline.

```text
JSON Files
│
▼
JsonLoader
│
▼
Graph Construction
│
▼
MetroSystem
│
▼
Services
│
▼
Algorithms
│
▼
Results
│
▼
Console Output
```

The graph is constructed only once during application startup.

Every algorithm operates on the same shared graph instance.

This guarantees consistency across all project rounds.

---

# Object Collaboration

Although every class has a specific responsibility, the application behaves as a collaborative system.

A typical routing request involves several independent objects.

```text
MetroSystem
│
└── RoutingService
        │
        ├── Select Algorithm
        │
        ├── Execute Algorithm
        │
        └── Return Result
                │
                ▼
            Console Output
```

Each object performs one task before delegating the next responsibility.

---

# Object Responsibilities

The following table summarizes the responsibilities of the major components.

| Component | Responsibility |
|------------|----------------|
| MetroSystem | Coordinates the entire application |
| Graph | Stores the metro network |
| Station | Represents a metro station |
| Edge | Represents a graph connection |
| JsonLoader | Reads JSON files |
| RoutingService | Routing algorithms |
| NetworkService | Network optimization algorithms |
| AnalysisService | Graph analysis |
| SearchService | Station search |
| Algorithms | Solve computational problems |
| Utils | Shared utilities |

No class should perform responsibilities belonging to another component.

---

# SOLID Principles

The architecture follows the SOLID principles as closely as possible.

Rather than treating SOLID as theoretical concepts, each principle is reflected in the project structure.

---

# Single Responsibility Principle

Every class should have exactly one reason to change.

Examples include:

```text
Graph
│
└── Stores graph data only

JsonLoader
│
└── Reads JSON files only

Logger
│
└── Writes logs only

Dijkstra
│
└── Computes shortest paths only

MetroSystem
│
└── Coordinates application workflow
```

Each class performs one well-defined task.

This greatly improves readability and maintainability.

---

# Open / Closed Principle

The system is designed to allow new functionality without modifying existing code.

Instead of editing old classes, developers create new implementations.

For example:

```text
IShortestPath
│
├── Dijkstra
├── BellmanFord
├── FloydWarshall
├── DAGShortestPath
└── AStar
```

Adding a future algorithm such as **Bidirectional Dijkstra** only requires implementing the same interface.

Existing algorithms remain untouched.

---

# Liskov Substitution Principle

Every implementation of an interface must behave consistently.

For example,

```text
IShortestPath
```

may point to

- Dijkstra
- BellmanFord
- AStar

without affecting the rest of the application.

Client code should never need to know which implementation is currently in use.

---

# Interface Segregation Principle

Instead of one large interface containing dozens of unrelated methods, the project defines several focused interfaces.

```text
Interfaces/

IGraph

IReachability

IShortestPath

IMST

IMaxFlow
```

Each algorithm implements only the interface relevant to its own problem domain.

This keeps interfaces small and easy to understand.

---

# Dependency Inversion Principle

High-level components never depend on concrete implementations.

Instead,

```text
RoutingService

↓

IShortestPath
```

rather than

```text
RoutingService

↓

Dijkstra
```

The routing service knows only the interface.

The actual implementation can change without affecting higher-level modules.

---

# Design Patterns

Several software design patterns naturally emerge from the architecture.

These patterns improve maintainability while keeping the implementation modular.

---

# Facade Pattern

The application exposes one central entry point.

```text
main()

↓

MetroSystem
```

Instead of interacting with dozens of independent classes, the application communicates only with `MetroSystem`.

This greatly simplifies the user interface.

---

# Strategy Pattern

Routing algorithms belong to the same family.

Instead of hardcoding one implementation,

the project selects a strategy at runtime.

```text
IShortestPath
│
├── Dijkstra
├── BellmanFord
├── FloydWarshall
└── AStar
```

Every algorithm becomes interchangeable.

---

# Dependency Injection

Services receive algorithm implementations through interfaces rather than creating them internally.

Conceptually,

```text
RoutingService

↓

IShortestPath
```

instead of

```text
RoutingService

↓

new Dijkstra()
```

This keeps services independent of concrete implementations and greatly simplifies testing.

---

# Factory (Possible Future Improvement)

As the number of algorithms grows, object creation can be centralized inside a factory.

Example:

```text
AlgorithmFactory

↓

CreateShortestPath()

↓

Dijkstra
```

or

```text
AlgorithmFactory

↓

CreateMST()

↓

Prim
```

Although not required in the initial implementation, the current architecture can easily accommodate this pattern.

---

# Repository-like Data Access

The graph behaves similarly to a repository.

Algorithms never manipulate JSON files directly.

Instead,

```text
JSON

↓

JsonLoader

↓

Graph

↓

Algorithms
```

This separates persistent data from computational logic.

---

# Why These Patterns?

These patterns were selected because they align naturally with the project goals.

They help:

- reduce coupling
- improve readability
- simplify testing
- isolate responsibilities
- support future expansion
- encourage code reuse

Most importantly, they allow every project round to extend the system without rewriting existing components.

---

# What's Next?

The final part of this document covers:

- Complete Application Workflow
- Module Interaction
- Testing Strategy
- Build Process
- Coding Guidelines
- Naming Conventions
- Future Improvements
- Final Architectural Notes

These sections complete the architectural documentation and provide practical guidelines for future development.



# Application Workflow

The application follows a predictable and structured execution flow.

Every operation in the system eventually follows the same sequence of steps.

```text
Application Start
│
├── Load Configuration
│
├── Load JSON Data
│
├── Construct Graph
│
├── Initialize Services
│
├── Create MetroSystem
│
├── Display Main Menu
│
├── Receive User Input
│
├── Execute Requested Operation
│
├── Display Result
│
└── Wait For Next Command
```

This workflow remains consistent regardless of the algorithm being executed.

Whether the user requests a shortest path, an MST, a max-flow computation, or a simulation, the execution pipeline remains the same.

---

# Routing Workflow

The routing subsystem follows a dedicated execution sequence.

```text
User Request
│
▼
MetroSystem
│
▼
RoutingService
│
▼
Select Routing Algorithm
│
▼
Execute Algorithm
│
▼
Query Graph Data
│
▼
Generate Route
│
▼
Return Result
│
▼
Display Output
```

This structure ensures that routing logic remains isolated from the user interface.

---

# Network Analysis Workflow

Network analysis operations follow a similar pattern.

```text
User Request
│
▼
MetroSystem
│
▼
NetworkService
│
▼
Select Analysis Algorithm
│
▼
Execute Algorithm
│
▼
Process Graph Data
│
▼
Generate Report
│
▼
Return Result
```

The user interface remains unaware of the underlying algorithm implementation.

---

# Simulation Workflow

Simulation modules introduce additional processing steps.

```text
Passenger Data
│
▼
PassengerSimulator
│
▼
Generate Events
│
▼
Dispatch Queue
│
▼
Platform Allocation
│
▼
Collect Statistics
│
▼
Generate Report
```

Simulation logic remains completely independent from graph algorithms.

---

# Testing Strategy

Testing is a fundamental part of the project architecture.

Every major subsystem should be tested independently.

The project follows a module-based testing strategy.

```text
tests/
│
├── GraphTests.cpp
├── RoutingTests.cpp
├── MSTTests.cpp
├── FlowTests.cpp
└── SimulationTests.cpp
```

This organization mirrors the actual project architecture.

---

# Testing Philosophy

The objective of testing is not merely to verify correctness.

Tests should also verify:

- Stability
- Consistency
- Interface contracts
- Error handling
- Edge cases
- Future compatibility

Every algorithm introduced in future rounds should reuse the same testing infrastructure whenever possible.

---

# Example Test Categories

## Graph Tests

Verify:

- Vertex insertion
- Edge insertion
- Neighbor retrieval
- Graph size
- Connectivity

---

## Routing Tests

Verify:

- Correct shortest paths
- Reachability
- Path reconstruction
- Invalid source nodes
- Invalid destination nodes

---

## MST Tests

Verify:

- Total MST weight
- Tree validity
- Disconnected graph handling

---

## Flow Tests

Verify:

- Maximum flow values
- Residual graph behavior
- Capacity constraints

---

## Simulation Tests

Verify:

- Event scheduling
- Queue ordering
- Passenger generation
- Statistical calculations

---

# Build Process

The project uses CMake as its build system.

The build workflow is intentionally simple.

```bash
mkdir build
cd build

cmake ..

cmake --build .

./QomMetro
```

The build system automatically discovers source files and links all required modules.

---

# External Dependencies

The project intentionally minimizes external dependencies.

Current dependency list:

```text
nlohmann/json
```

Reasons:

- Header-only
- Lightweight
- Portable
- Industry standard
- Easy integration with CMake

Reducing dependency count simplifies maintenance and improves portability.

---

# Error Handling Philosophy

Errors should be handled consistently throughout the project.

General guidelines include:

- Validate user input
- Validate JSON data
- Validate graph integrity
- Avoid undefined behavior
- Fail gracefully whenever possible

The system should always provide meaningful error messages rather than crashing unexpectedly.

---

# Logging Strategy

Logging is centralized through the `Logger` utility.

Potential log categories include:

```text
INFO

WARNING

ERROR

DEBUG
```

Centralized logging simplifies debugging and improves maintainability.

---

# Coding Guidelines

To maintain consistency throughout the codebase, developers should follow a common coding style.

---

## Naming Conventions

### Classes

Use PascalCase.

Examples:

```text
Graph

MetroSystem

RoutingService

PassengerSimulator
```

---

### Interfaces

Prefix interfaces with `I`.

Examples:

```text
IGraph

IShortestPath

IMST

IMaxFlow
```

---

### Methods

Use camelCase.

Examples:

```text
findShortestPath()

loadStations()

calculateMaxFlow()
```

---

### Variables

Use descriptive names.

Examples:

```text
stationId

totalDistance

maximumFlow
```

Avoid abbreviations whenever possible.

---

### Constants

Use uppercase naming.

Examples:

```text
MAX_CAPACITY

DEFAULT_TIMEOUT

PI
```

---

# Source Code Organization Rules

To keep the project maintainable, several rules should always be respected.

### Rule 1

Algorithms must not contain user interface code.

Incorrect:

```text
Dijkstra
│
└── printf(...)
```

---

### Rule 2

Graph classes must not contain algorithm implementations.

Incorrect:

```text
Graph
│
├── Dijkstra()
├── Prim()
└── FloydWarshall()
```

---

### Rule 3

Services should coordinate behavior rather than perform computations.

Algorithms belong inside algorithm classes.

---

### Rule 4

Input/output logic must remain isolated.

Algorithms should never read JSON files directly.

---

### Rule 5

Every module should have a clear responsibility.

If a class gains multiple unrelated responsibilities, consider splitting it.

---

# Extensibility

A major design goal of this project is future expansion.

The architecture should support adding new functionality with minimal modification to existing code.

Examples:

```text
New Algorithm

↓

Implement Interface

↓

Register Service

↓

Expose Through Menu
```

No existing algorithm should require modification.

---

# Potential Future Improvements

The current architecture leaves room for significant expansion.

Possible future additions include:

### Advanced Routing

- Bidirectional Dijkstra
- ALT Algorithm
- Contraction Hierarchies
- Multi-Criteria Routing

---

### Simulation Enhancements

- Real-time passenger generation
- Congestion simulation
- Train delay modeling
- Demand forecasting

---

### Visualization

- Graph visualization
- Interactive metro map
- Route animation
- Performance dashboards

---

### Software Engineering Improvements

- Dependency injection container
- Plugin architecture
- Benchmark framework
- Configuration system

---

# Architectural Advantages

The current architecture provides several important benefits.

### Maintainability

Responsibilities are clearly separated.

### Extensibility

New algorithms can be added easily.

### Testability

Modules can be tested independently.

### Reusability

Algorithms operate on shared abstractions.

### Scalability

The architecture can grow without major redesign.

### Readability

The project structure mirrors the conceptual design.

---

# Final Notes

The most important architectural rule of this project is:

> **Store data in the graph. Implement logic inside algorithms. Coordinate behavior through services. Control the application through MetroSystem.**

This principle guides every design decision throughout the project.

By following this architecture, the Qom Metro Algorithmic System remains modular, extensible, maintainable, and capable of supporting all project rounds while preserving a clean and professional codebase.

---

# Conclusion

The architecture presented in this document is designed not only to satisfy the requirements of the Algorithm Design course, but also to demonstrate professional software engineering practices.

Every component has a clearly defined responsibility.

Every algorithm operates on shared abstractions.

Every module can evolve independently.

The result is a system that remains understandable today while still being flexible enough to support future growth.

**Good algorithms solve problems. Great architecture makes them reusable.**