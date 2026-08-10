#include "Algorithms/Levenshtein.hpp"

#include <algorithm>
#include <vector>

using namespace std;

namespace QomMetro::Algorithms {

int Levenshtein::distance(const string& a, const string& b){

    size_t n = a.size();
    size_t m = b.size();

    
    if (n == 0) return static_cast<int>(m);
    if (m == 0) return static_cast<int>(n);

   
    const string& shorter = (n <= m) ? a : b;
    const string& longer = (n <= m) ? b : a;
    size_t row_len = shorter.size();

    vector<int> previous_row(row_len + 1);
    vector<int> current_row(row_len + 1);

    for (size_t j = 0; j <= row_len; ++j){
        previous_row[j] = static_cast<int>(j);
    }

    for(size_t i = 1; i <= longer.size(); ++i){

        current_row[0] = static_cast<int>(i);

        for(size_t j = 1; j <= row_len; ++j){
            
            if(longer[i - 1] == shorter[j - 1]){
                current_row[j] = previous_row[j - 1]; 
            } 
            else{

                int deletion = previous_row[j] + 1;
                int insertion = current_row[j - 1] + 1;
                int substitution = previous_row[j - 1] + 1;
                current_row[j] = min({deletion, insertion, substitution});
            }
        }

        swap(previous_row, current_row);
    }

    return previous_row[row_len];
}

vector<Search_Match> Levenshtein::find_closest(
    const Core::IGraph& graph, const string& query){



    vector<Search_Match> best_matches;
    int best_distance = -1;

    for(const Core::Station& station : graph.all_stations()){

        int d = distance(query, station.name());

        if(best_distance == -1 || d < best_distance){

            best_distance = d;
            best_matches.clear();
            best_matches.push_back(Search_Match{station.id(), station.name(), d});
        } 
        else if(d == best_distance){
            best_matches.push_back(Search_Match{station.id(), station.name(), d});
        }
    }

    return best_matches;
}

} // namespace QomMetro::Algorithms