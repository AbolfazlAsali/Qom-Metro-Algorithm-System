#include <iostream>
#include <limits>
#include <string>

#include "Core/Metro_System.hpp"
#include "Core/Weight_Type.hpp"
#include "IO/Json_Loader.hpp"
#include "Utils/Constants.hpp"
#include "Utils/Logger.hpp"

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
    cout << "(Note: edge capacities default to unlimited until data/capacities.json\n"
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

void print_menu(){

    cout << "\n=== QomMetro CLI ===\n"
              << "1. List stations\n"
              << "2. Check reachability (T1.2)\n"
              << "3. Find shortest path (T1.3)\n"
              << "4. Build minimum spanning tree (T2.1)\n"
              << "5. Compute max flow (T4.2)\n"
              << "6. Find critical stations / bridges (T4.3)\n"
              << "7. Search station name (T4.6)\n"
              << "0. Exit\n"
              << "Choice: ";
}

} // namespace

int main(){

    try{

        QomMetro::IO::Json_Loader loader;
        Metro_System system(
            loader,
            QomMetro::Utils::Default_Stations_File,
            QomMetro::Utils::Default_Edges_File);

        QomMetro::Utils::Logger::info("QomMetro system ready.");

        while(true){

            print_menu();
            int choice = readInt("");

            switch(choice){
                
                case 1: print_stations(system); break;
                case 2: handle_reachability(system); break;
                case 3: handle_shortest_path(system); break;
                case 4: handle_mst(system); break;
                case 5: handle_max_flow(system); break;
                case 6: handle_critical_stations(system); break;
                case 7: handle_search(system); break;
                case 0: return 0;
                default: cout << "Unknown choice.\n"; break;
            }
        }
    } 
    catch(const exception& e){
        QomMetro::Utils::Logger::error(string("Fatal error: ") + e.what());
        return 1;
    }
}