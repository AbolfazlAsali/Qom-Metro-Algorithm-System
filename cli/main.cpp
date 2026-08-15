#include <iostream>
#include <limits>
#include <string>

#include "Algorithms/AStar.hpp"
#include "Algorithms/Bellman_Ford.hpp"
#include "Algorithms/DAG_Shortest_Path.hpp"
#include "Algorithms/Dijkstra.hpp"
#include "Algorithms/Kruskal.hpp"
#include "Algorithms/Prim.hpp"
#include "Core/Metro_System.hpp"
#include "Core/Weight_Type.hpp"
#include "IO/Json_Loader.hpp"
#include "Simulation/Dispatch_Queue_Manager.hpp"
#include "Simulation/Passenger_Simulator.hpp"
#include "Simulation/Platform_Allocator.hpp"
#include "Utils/Constants.hpp"
#include "Utils/Logger.hpp"
#include "Utils/Timer.hpp"

using namespace std;

namespace {

using QomMetro::Core::Metro_System;
using QomMetro::Core::Weight_Type;

void print_stations(const Metro_System& system){

    cout << "\n--- Stations ---\n";
    for(const auto& station : system.graph().all_stations()){
        cout << station.id() << ": " << station.name() << "\n";
    }
}

void print_path(const vector<int>& path){

    for(size_t i = 0; i < path.size(); ++i){

        cout << path[i];
        if(i + 1 < path.size()){
            cout << " -> ";
        }
    }
    cout << "\n";
}

int readInt(const string& prompt){

    cout << prompt;
    int value;

    while(!(cin >> value)){

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Please enter a valid integer: ";
    }
    return value;
}

Weight_Type read_weight_type(){

    cout << "Weight type (0 = Distance, 1 = Time): ";
    int choice = readInt("");
    return (choice == 1) ? Weight_Type::Time : Weight_Type::Distance;
}




void handle_reachability(const Metro_System& system){

    int from = readInt("From station id: ");
    int to = readInt("To station id: ");

    auto result = system.check_reachability(from, to);
    if(!result.reachable){
        cout << "No path exists between " << from << " and " << to << ".\n";
        return;
    }
    cout << "Reachable. Path: ";
    print_path(result.path);
}

void handle_shortest_path(const Metro_System& system){

    int from = readInt("From station id: ");
    int to = readInt("To station id: ");
    Weight_Type weight_Type = read_weight_type();

    auto result = system.find_shortest_path(from, to, weight_Type);
    if(!result.reachable){
        cout << "No path exists between " << from << " and " << to << ".\n";
        return;
    }
    cout << "Path: ";
    print_path(result.path);
    cout << "Total cost: " << result.total_cost << "\n";
    cout << "Nodes expanded: " << result.nodes_expanded << "\n";
}

void handle_mst(const Metro_System& system){

    Weight_Type weight_Type = read_weight_type();
    auto result = system.build_minimum_spanning_tree(weight_Type);

    cout << (result.success ? "Spanning tree" : "Spanning FOREST (graph is disconnected)")
              << " with total weight " << result.total_weight << ":\n";
    for(const auto& edge : result.edges){
        cout << "  " << edge.from << " -- " << edge.to << " (" << edge.weight << ")\n";
    }
}

void handle_max_flow(const Metro_System& system){

    int source = readInt("Source station id: ");
    int sink = readInt("Sink station id: ");

    auto result = system.compute_max_flow(source, sink);
    cout << "Max flow: " << result.max_flow << "\n";
    cout << "(Note: edge capacities default_ to unlimited until data/capacities.json\n"
              << " is loaded and wired in -- see README.md, open item on T4.2 schema.)\n";
}

void handle_critical_stations(const Metro_System& system){

    auto result = system.find_critical_stations();

    cout << "Articulation points: ";
    for(int id : result.articulation_points){
        cout << id << " ";
    }

    cout << "\nBridges:\n";
    for(const auto& bridge : result.bridges){
        cout << "  " << bridge.from << " -- " << bridge.to << "\n";
    }
}

void handle_search(const Metro_System& system){

    cout << "Station name (typo-tolerant): ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string query;
    getline(cin, query);

    auto matches = system.find_station(query);
    cout << "Closest match(es):\n";
    for(const auto& match : matches){
        cout << "  [" << match.station_id << "] " << match.station_name
                  << " (edit distance " << match.edit_distance << ")\n";
    }
}

void handle_compare_mst(const Metro_System& system){

    Weight_Type weight_type = read_weight_type();

    QomMetro::Algorithms::Kruskal kruskal;
    QomMetro::Algorithms::Prim prim;
    QomMetro::Utils::Timer timer;

    timer.start();
    auto kruskal_result = kruskal.run(system.graph(), weight_type);
    timer.stop();
    double kruskal_ms = timer.elapsed_milli_seconds();

    timer.start();
    auto prim_result = prim.run(system.graph(), weight_type);
    timer.stop();
    double prim_ms = timer.elapsed_milli_seconds();

    cout << "Kruskal: total weight " << kruskal_result.total_weight
              << ", " << kruskal_ms << " ms\n";
    cout << "Prim:    total weight " << prim_result.total_weight
              << ", " << prim_ms << " ms\n";
}


void handle_express_line(const QomMetro::Core::Graph& express_graph){

    int from = readInt("From station id (express line): ");
    int to = readInt("To station id (express line): ");
    Weight_Type weight_type = read_weight_type();

    QomMetro::Algorithms::Dag_Shortest_Path dag;
    auto result = dag.run(express_graph, from, to, weight_type);

    if(!result.reachable){
        cout << "No express-line path exists between " << from << " and " << to << ".\n";
        return;
    }
    cout << "Express path: ";
    print_path(result.path);
    cout << "Total cost: " << result.total_cost << "\n";
}

void handle_bellman_ford(const Metro_System& system){

    int from = readInt("From station id: ");
    int to = readInt("To station id: ");
    Weight_Type weight_type = read_weight_type();

    QomMetro::Algorithms::Bellman_Ford bellman_ford;
    auto result = bellman_ford.run(system.graph(), from, to, weight_type);

    if(result.has_negative_cycle){
        cout << "Negative cycle detected -- shortest paths are undefined "
                     "under the current incentive weights.\n";
        return;
    }
    if(!result.reachable){
        cout << "No path exists between " << from << " and " << to << ".\n";
        return;
    }
    cout << "Path (with incentives applied): ";
    print_path(result.path);
    cout << "Total cost: " << result.total_cost << "\n";
}

void handle_platform_allocation(const QomMetro::IO::Json_Loader& loader){

    try{
        auto trains = loader.load_Trains(QomMetro::Utils::Default_Trains_File);

        QomMetro::Simulation::Platform_Allocator allocator;
        auto result = allocator.allocate(trains);

        cout << "Scheduled " << result.scheduled.size() << " of " << trains.size()
                  << " trains on the shared platform:\n";

        for(const auto& train : result.scheduled){

            cout << "  Train " << train.id << ": " << train.arrival_min
                      << " - " << train.departure_min << "\n";
        }
    } 
    catch(const exception& e){
        cout << "Could not run platform allocation: " << e.what() << "\n";
    }
}

void handle_dispatch_queue(const QomMetro::IO::Json_Loader& loader){

    try{
        auto trains = loader.load_Trains(QomMetro::Utils::Default_Trains_File);

        QomMetro::Simulation::Dispatch_Queue_Manager dispatcher;
        for(const auto& train : trains){        
            dispatcher.add_or_update(train.id, -train.delay_min);
        }


        cout << "Dispatch order (most delayed first):\n";
        while(!dispatcher.empty()){

            auto next = dispatcher.dispatch_next();
            if(next.has_value()){
                cout << "  Train " << *next << "\n";
            }
        }
    } 
    catch(const exception& e){
        cout << "Could not run dispatch queue demo: " << e.what() << "\n";
    }
}

void handle_statistics(const Metro_System& system, const QomMetro::IO::Json_Loader& loader){

    try{
        auto visits = loader.load_Visits(QomMetro::Utils::Default_Passengers_File);
        auto result = system.analyze_operations(visits);

        cout << "Average daily trips: " << result.average_daily_trips << "\n";
        cout << "Stations by visit count (most to least visited):\n";
        for(int stationId : result.Stations_ByVisit_Count_Desc){
            cout << "  " << stationId << ": " << system.graph().station(stationId).name() << "\n";
        }
    } 
    catch(const exception& e){
        cout << "Could not compute statistics: " << e.what() << "\n";
    }
}

void handle_passenger_simulation(){

    QomMetro::Simulation::Simulation_Config config;
    config.duration_min = readInt("Simulation duration (minutes): ");
    cout << "Average arrivals per minute: ";
    cin >> config.arrival_rate_per_min;
    cout << "Average service time per passenger (minutes): ";
    cin >> config.mean_service_time_min;
    config.gate_count = readInt("Number of gates: ");
    config.random_seed = static_cast<unsigned int>(readInt("Random seed: "));

    QomMetro::Simulation::Passenger_Simulator simulator;
    auto result = simulator.simulate(config);

    cout << "Total passengers processed: " << result.total_passengers << "\n";
    cout << "Average wait: " << result.average_wait_min << " min\n";
    cout << "Max wait: " << result.max_wait_min << " min\n";
}


void handle_precomputed_shortest_path(const Metro_System& system){

    int from = readInt("From station id: ");
    int to = readInt("To station id: ");
    Weight_Type weight_type = read_weight_type();

    auto result = system.shortest_path_precomputed(from, to, weight_type);
    if(!result.reachable){
        cout << "No path exists between " << from << " and " << to << ".\n";
        return;
    }
    cout << "Path (via Floyd-Warshall cache): ";
    print_path(result.path);
    cout << "Total cost: " << result.total_cost << "\n";
}

void handle_emergency_teams(const Metro_System& system){

    auto result = system.place_emergency_teams();
    cout << "Emergency teams placed at " << result.team_station_ids.size() << " station(s):\n";
    for(int id : result.team_station_ids){
        cout << "  " << id << ": " << system.graph().station(id).name() << "\n";
    }
}

void handle_compare_dijkstra_Astar(const Metro_System& system){

    int from = readInt("From station id: ");
    int to = readInt("To station id: ");
    Weight_Type weight_type = read_weight_type();

    QomMetro::Algorithms::Dijkstra dijkstra;
    QomMetro::Algorithms::AStar astar;
    QomMetro::Utils::Timer timer;

    timer.start();
    auto dijkstra_result = dijkstra.run(system.graph(), from, to, weight_type);
    timer.stop();
    double dijkstra_ms = timer.elapsed_milli_seconds();

    timer.start();
    auto astar_result = astar.run(system.graph(), from, to, weight_type);
    timer.stop();
    double astar_ms = timer.elapsed_milli_seconds();

    cout << "Dijkstra: cost " << dijkstra_result.total_cost
              << ", nodes expanded " << dijkstra_result.nodes_expanded
              << ", " << dijkstra_ms << " ms\n";
    cout << "AStar:    cost " << astar_result.total_cost
              << ", nodes expanded " << astar_result.nodes_expanded
              << ", " << astar_ms << " ms\n";
}


void print_menu(){

    cout << "\n=== QomMetro CLI ===\n"
              << "1.  List stations\n"
              << "2.  Check reachability (T1.2)\n"
              << "3.  Find shortest path (T1.3, Dijkstra)\n"
              << "4.  Build minimum spanning tree (T2.1, Kruskal)\n"
              << "5.  Compare Kruskal vs Prim (T2.1)\n"
              << "6.  Express line shortest path (T2.3)\n"
              << "7.  Shortest path with incentives / negative-cycle check (T2.4)\n"
              << "8.  Platform allocation demo (T3.1)\n"
              << "9.  Dispatch queue demo (T3.2)\n"
              << "10. Operational statistics (T3.3)\n"
              << "11. Passenger arrival simulation (T3.4)\n"
              << "12. Precomputed shortest path (T4.1, Floyd-Warshall)\n"
              << "13. Compute max flow (T4.2)\n"
              << "14. Find critical stations / bridges (T4.3)\n"
              << "15. Place emergency teams (T4.4, bonus)\n"
              << "16. Search station name (T4.6)\n"
              << "17. Compare Dijkstra vs AStar (Round 5)\n"
              << "0.  Exit\n"
              << "Choice: ";
}
} // namespace

int main(){

    try{

        QomMetro::IO::Json_Loader loader;
        Metro_System system(

            loader,
            QomMetro::Utils::Default_Stations_File,
            QomMetro::Utils::Default_Edges_File,
            QomMetro::Utils::Default_Capacities_File,
            QomMetro::Utils::Default_Incentives_File);

        auto express_graph = loader.load_Directed_Graph(
            QomMetro::Utils::Default_Stations_File,
            QomMetro::Utils::Default_ExpressEdges_File);

        QomMetro::Utils::Logger::info("QomMetro system ready.");

        while(true){

            print_menu();
            int choice = readInt("");

            switch(choice){
                case 1:  print_stations(system); break;
                case 2:  handle_reachability(system); break;
                case 3:  handle_shortest_path(system); break;
                case 4:  handle_mst(system); break;
                case 5:  handle_compare_mst(system); break;
                case 6:  handle_express_line(*express_graph); break;
                case 7:  handle_bellman_ford(system); break;
                case 8:  handle_platform_allocation(loader); break;
                case 9:  handle_dispatch_queue(loader); break;
                case 10: handle_statistics(system, loader); break;
                case 11: handle_passenger_simulation(); break;
                case 12: handle_precomputed_shortest_path(system); break;
                case 13: handle_max_flow(system); break;
                case 14: handle_critical_stations(system); break;
                case 15: handle_emergency_teams(system); break;
                case 16: handle_search(system); break;
                case 17: handle_compare_dijkstra_Astar(system); break;
                case 0:  return 0;
                default: cout << "Unknown choice.\n"; break;
            }
        }
    } 
    catch(const exception& e){
        QomMetro::Utils::Logger::error(string("Fatal error: ") + e.what());
        return 1;
    }
}