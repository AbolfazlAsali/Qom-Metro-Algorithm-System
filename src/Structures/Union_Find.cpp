#include "Structures/Union_Find.hpp"

namespace QomMetro::Structures {

Union_Find::Union_Find(int size)
    : parent_(size), rank_(size, 0){

    for(int i = 0; i < size; ++i){
        parent_[i] = i;
    }
}

int Union_Find::find(int x){

    if (parent_[x] != x) {
        parent_[x] = find(parent_[x]); 
    }
    return parent_[x];
}

bool Union_Find::unite(int a, int b){

    int rootA = find(a);
    int rootB = find(b);

    if (rootA == rootB){
        return false;
    }


    if(rank_[rootA] < rank_[rootB]){
        parent_[rootA] = rootB;
    } 
    else if(rank_[rootA] > rank_[rootB]){
        parent_[rootB] = rootA;
    } 
    else{
        
        parent_[rootB] = rootA;
        rank_[rootA]++;
    }
    return true;
}

bool Union_Find::connected(int a, int b) {
    return find(a) == find(b);
}

} // namespace QomMetro::Structures