#include "IO/Json_Loader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "Core/Edge.hpp"
#include "Core/Station.hpp"
#include "Utils/Logger.hpp"

namespace QomMetro::IO{

using json = nlohmann::json;
using QomMetro::Core::Coordinates;
using QomMetro::Core::Edge;
using QomMetro::Core::Graph;
using QomMetro::Core::Station;

namespace{


json read_json_file(const std::string& path){

    std::ifstream file(path);
    if(!file.is_open()){
        throw std::runtime_error("Json_Loader: could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    try{
        return json::parse(buffer.str());
    } 
    catch(const json::parse_error& e){

        throw std::runtime_error(
            "Json_Loader: malformed JSON in '" + path + "': " + e.what());
    }
}

void loadStations(const json& stations_json, Graph& graph){
    
    if(!stations_json.contains("stations")){

        throw std::runtime_error(
            "Json_Loader: stations file is missing the top-level \"stations\" array");
    }

    for(const auto& Station_Json : stations_json.at("stations")){

        int id = Station_Json.at("id").get<int>();
        std::string name = Station_Json.at("name").get<std::string>();

        bool hasLat = Station_Json.contains("lat");
        bool hasLng = Station_Json.contains("lng");

        if(hasLat && hasLng){

            Coordinates coords{

                Station_Json.at("lat").get<double>(),
                Station_Json.at("lng").get<double>()
            };
            graph.add_station(Station(id, std::move(name), coords));
        } 
        else{
            graph.add_station(Station(id, std::move(name)));
        }
    }
}



void load_edges(const json& edges_json, Graph& graph){

    if(!edges_json.contains("edges")){

        throw std::runtime_error(
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

}

std::unique_ptr<Graph> Json_Loader::load_Graph
(
    const std::string& stations_file_path,
    const std::string& edges_file_path

) const{

    auto graph = std::make_unique<Graph>();

    try{

        json stationsJson = read_json_file(stations_file_path);
        loadStations(stationsJson, *graph);

        json edgesJson = read_json_file(edges_file_path);
        load_edges(edgesJson, *graph);
    } 
    catch (const json::exception& e){
       
        throw std::runtime_error(
            std::string("Json_Loader: invalid data - ") + e.what());
    }

    Utils::Logger::info(
        "Json_Loader: loaded " + std::to_string(graph->station_count()) + " stations.");

    return graph;
}

} // namespace QomMetro::IO