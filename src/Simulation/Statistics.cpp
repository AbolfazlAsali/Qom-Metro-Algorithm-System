#include "Simulation/Statistics.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <utility>

using namespace std;

namespace QomMetro::Simulation {

Statistics_Result Statistics::analyze(const vector<Visit_Record>& visits) const{

    Statistics_Result result;

    if(visits.empty()){
        return result; 
    }

    unordered_set<int> unique_days;
    unordered_map<int, int> visit_count_byStation;

    for(const Visit_Record& visit : visits){

        unique_days.insert(visit.day);
        visit_count_byStation[visit.station_id]++;
    }

    result.average_daily_trips =
        static_cast<double>(visits.size()) / static_cast<double>(unique_days.size());

  
    vector<pair<int, int>> station_counts(
        visit_count_byStation.begin(), visit_count_byStation.end());

    sort
    (
        station_counts.begin(), station_counts.end(),
        [](const pair<int, int>& a, const pair<int, int>& b) {
            return a.second > b.second;
        }
    );

    result.Stations_ByVisit_Count_Desc.reserve(station_counts.size());
    for(const auto& [station_id, count] : station_counts){
        result.Stations_ByVisit_Count_Desc.push_back(station_id);
    }

    return result;
}

} // namespace QomMetro::Simulation