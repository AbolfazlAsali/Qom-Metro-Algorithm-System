#include "Core/Station.hpp"

#include <cassert>
#include <utility>

using namespace std;

namespace QomMetro::Core{

Station::Station(int id, string name)
    : id_(id), name_(move(name)), coordinates_(nullopt){
}

Station::Station(int id, string name, Coordinates coordinates)
    : id_(id), name_(move(name)), coordinates_(coordinates){
}

int Station::id() const{
    return id_;
}

const string& Station::name() const{
    return name_;
}

bool Station::has_coordinates() const{
    return coordinates_.has_value();
}

const Coordinates& Station::coordinates() const{
    assert(coordinates_.has_value() && "coordinates() called but this Station has none");
    return *coordinates_;
}

} // namespace QomMetro::Core