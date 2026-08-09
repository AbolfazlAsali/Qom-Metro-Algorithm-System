#pragma once

#include "IO/File_Loader.hpp"

namespace QomMetro::IO {

// Expected schema:
//
//   stations.json:
//   { "stations": [
//       { "id": 0, "name": "...", "lat": 34.64, "lng": 50.87 },  // lat/lng optional
//       { "id": 1, "name": "..." }
//   ]}
//
//   edges.json:
//   { "edges": [
//       { "from": 0, "to": 1, "distance_km": 2.5, "time_min": 5 }
//   ]}
class Json_Loader : public File_Loader{

public:

    std::unique_ptr<Core::Graph> load_Graph(

        const std::string& stations_file_path,
        const std::string& edges_file_path
    
    ) const override;
};

} // namespace QomMetro::IO