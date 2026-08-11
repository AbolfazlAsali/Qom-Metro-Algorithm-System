#pragma once

#include <vector>

namespace QomMetro::Simulation {

// One recorded passenger visit to a station.
struct Visit_Record{

    int station_id;
    int day;
};

// Aggregated statistics computed by Statistics::analyze() (T3.3).
struct Statistics_Result{

    double average_daily_trips = 0.0;

    // Station ids ordered from most-visited to least-visited. The k-th
    // most-visited station is stationsByVisitCountDesc[k - 1].
    std::vector<int> Stations_ByVisit_Count_Desc;
};


class Statistics {
public:

    Statistics_Result analyze(const std::vector<Visit_Record>& visits) const;
};

} // namespace QomMetro::Simulation