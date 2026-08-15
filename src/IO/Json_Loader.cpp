#include "IO/Json_Loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "Core/Edge.hpp"
#include "Core/Station.hpp"
#include "Utils/Logger.hpp"

using namespace std;

namespace QomMetro::IO{

using json = nlohmann::json;
using QomMetro::Core::Coordinates;
using QomMetro::Core::Edge;
using QomMetro::Core::Graph;
using QomMetro::Core::Station;

namespace{


json read_json_file(const string& path){

    ifstream file(path);
    if(!file.is_open()){
        throw runtime_error("Json_Loader: could not open file: " + path);
    }

    stringstream buffer;
    buffer << file.rdbuf();

    try{
        return json::parse(buffer.str());
    } 
    catch(const json::parse_error& e){

        throw runtime_error(
            "Json_Loader: malformed JSON in '" + path + "': " + e.what());
    }
}

void load_Stations(const json& stations_json, Graph& graph){
    
    if(!stations_json.contains("stations")){

        throw runtime_error(
            "Json_Loader: stations file is missing the top-level \"stations\" array");
    }

    for(const auto& Station_Json : stations_json.at("stations")){

        int id = Station_Json.at("id").get<int>();
        string name = Station_Json.at("name").get<string>();

        bool hasLat = Station_Json.contains("lat");
        bool hasLng = Station_Json.contains("lng");

        if(hasLat && hasLng){

            Coordinates coords{

                Station_Json.at("lat").get<double>(),
                Station_Json.at("lng").get<double>()
            };
            graph.add_station(Station(id, move(name), coords));
        } 
        else{
            graph.add_station(Station(id, move(name)));
        }
    }
}



void load_edges(const json& edges_json, Graph& graph, bool bidirectional){

    if(!edges_json.contains("edges")){
        throw runtime_error(
            "Json_Loader: edges file is missing the top-level \"edges\" array");
    }

    for(const auto& Edge_Json : edges_json.at("edges")){

        int from = Edge_Json.at("from").get<int>();
        int to = Edge_Json.at("to").get<int>();
        double distance_Km = Edge_Json.at("distance_km").get<double>();
        double time_Min = Edge_Json.at("time_min").get<double>();

        graph.add_edge(from, Edge(to, distance_Km, time_Min));
        graph.add_edge(to, Edge(from, distance_Km, time_Min)); 
    }
}

template <typename SetterFn>
void apply_edge_overlay(

    const string& file_path,
    const string& top_level_key,
    const string& value_key,
    Graph& graph,
    SetterFn setter

) {


    json overlay_json = read_json_file(file_path);

    if(!overlay_json.contains(top_level_key)){

        throw runtime_error(
            "Json_Loader: '" + file_path + "' is missing the top-level \"" +
            top_level_key + "\" array");
    }

    try{

        for(const auto& entryJson : overlay_json.at(top_level_key)){

            int from = entryJson.at("from").get<int>();
            int to = entryJson.at("to").get<int>();
            double value = entryJson.at(value_key).get<double>();

            bool applied = setter(graph, from, to, value);
            if(!applied){
           
                Utils::Logger::warning(
                    "Json_Loader: '" + file_path + "' references edge " +
                    to_string(from) + " -> " + to_string(to) +
                    ", which does not exist in the graph. Skipped.");
            }
        }
    } 
    catch(const json::exception& e){
        throw runtime_error(
            string("Json_Loader: invalid data in '") + file_path + "' - " + e.what());
    }
}

} // namespace

unique_ptr<Graph> Json_Loader::load_Graph(
    const string& stations_file_path,
    const string& edges_file_path

) const{


    auto graph = make_unique<Graph>();
    try{

        json stations_json = read_json_file(stations_file_path);
        load_Stations(stations_json, *graph);

        json edges_eson = read_json_file(edges_file_path);
        load_edges(edges_eson, *graph, true);
    } 
    catch (const json::exception& e){
       
        throw runtime_error(
            string("Json_Loader: invalid data - ") + e.what());
    }

    Utils::Logger::info(
        "Json_Loader: loaded " + to_string(graph->station_count()) + " stations.");

    return graph;
}



unique_ptr<Graph> Json_Loader::load_Directed_Graph(
    const string& stations_file_path,
    const string& edges_file_path

) const{



    auto graph = make_unique<Graph>();

    try{
        json stations_json = read_json_file(stations_file_path);
        load_Stations(stations_json, *graph);

        json edges_eson = read_json_file(edges_file_path);
        load_edges(edges_eson, *graph, false);
    } 
    catch(const json::exception& e){
        throw runtime_error(
            string("Json_Loader: invalid express graph data - ") + e.what());
    }

    Utils::Logger::info(
        "Json_Loader: loaded directed (express) graph with " +
        to_string(graph->station_count()) + " stations.");

    return graph;
}



void Json_Loader::load_Capacities(
    const string& capacities_file_path, 
    Graph& graph

) const{


    apply_edge_overlay(
        capacities_file_path, "capacities", "capacity", graph,
        [](Graph& g, int from, int to, double value) {
            return g.set_edge_capacity(from, to, value);
        });

    Utils::Logger::info("Json_Loader: capacities loaded from " + capacities_file_path);
}




void Json_Loader::load_Incentives(
    const string& incentives_file_path, 
    Graph& graph

) const{


    apply_edge_overlay(
        incentives_file_path, "incentives", "bonus", graph,
        [](Graph& g, int from, int to, double value) {
            return g.set_edge_bonus(from, to, value);
        });

    Utils::Logger::info("Json_Loader: incentives loaded from " + incentives_file_path);
}




vector<Simulation::Train> Json_Loader::load_Trains(const string& path) const{


    json trains_json = read_json_file(path);
    if(!trains_json.contains("trains")){
        throw runtime_error(
            "Json_Loader: '" + path + "' is missing the top-level \"trains\" array");
    }

    vector<Simulation::Train> trains;
    try{

        for(const auto& train_json : trains_json.at("trains")){

            Simulation::Train train;
            train.id = train_json.at("id").get<int>();
            train.arrival_min = train_json.at("arrival_min").get<double>();
            train.departure_min = train_json.at("departure_min").get<double>();
            train.delay_min = train_json.value("delay_min", 0.0); 
            trains.push_back(train);
        }
    } 
    catch(const json::exception& e){
        throw runtime_error(
            string("Json_Loader: invalid trains data - ") + e.what());
    }

    Utils::Logger::info(
        "Json_Loader: loaded " + to_string(trains.size()) + " trains from " + path);

    return trains;
}





vector<Simulation::Visit_Record> Json_Loader::load_Visits(const string& path) const{

    json visits_json = read_json_file(path);
    if(!visits_json.contains("visits")){
        throw runtime_error(
            "Json_Loader: '" + path + "' is missing the top-level \"visits\" array");
    }


    vector<Simulation::Visit_Record> visits;
    try{
        for(const auto& visit_json : visits_json.at("visits")){

            Simulation::Visit_Record visit;
            visit.station_id = visit_json.at("station_id").get<int>();
            visit.day = visit_json.at("day").get<int>();
            visits.push_back(visit);
        }
    } 
    catch(const json::exception& e){
        throw runtime_error(
            string("Json_Loader: invalid visits data - ") + e.what());
    }

    Utils::Logger::info(
        "Json_Loader: loaded " + to_string(visits.size()) + " visit records from " + path);

    return visits;
}
} // namespace QomMetro::IO