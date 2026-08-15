#pragma once

#include <memory>
#include <string>

#include "Core/Graph.hpp"

namespace QomMetro::IO{


class File_Loader {
public:

    virtual ~File_Loader() = default;

    virtual std::unique_ptr <Core::Graph> load_Graph(

        const std::string& stations_file_path,
        const std::string& edges_file_path

    ) const = 0;



    virtual void load_Capacities(

        const std::string& capacities_file_path,
        Core::Graph& graph
    
    ) const = 0;



    virtual void load_Incentives(

        const std::string& incentives_file_path,
        Core::Graph& graph
    
    ) const = 0;



    virtual std::unique_ptr<Core::Graph> load_Directed_Graph(

        const std::string& stations_file_path,
        const std::string& edges_file_path
    
    ) const = 0;
};

} // namespace QomMetro::IO