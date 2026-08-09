#pragma once

#include <optional>
#include <string>

namespace QomMetro::Core {


struct Coordinates {
    double latitude;
    double longitude;
};

// A single metro station: an immutable id/name pair, plus optional
// geographic coordinates.
class Station {

public:

    Station(int id, std::string name);
    Station(int id, std::string name, Coordinates coordinates);

    int id() const;
    const std::string& name() const;

    bool has_coordinates() const;

    const Coordinates& coordinates() const;

private:

    int id_;
    std::string name_;
    std::optional<Coordinates> coordinates_;
};

} // namespace QomMetro::Core