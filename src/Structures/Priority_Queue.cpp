#include "Structures/Priority_Queue.hpp"

namespace QomMetro::Structures {

void Priority_Queue::push_or_decrease(int id, double priority){

    auto it = best_priority_.find(id);
    if(it != best_priority_.end() && it->second <= priority){
        return; 
    }
    best_priority_[id] = priority;
    heap_.push(priority, id);
}

void Priority_Queue::discard_stale_top(){

    while(!heap_.empty()){

        const Heap_Item& top = heap_.peek();
        auto it = best_priority_.find(top.id);

        bool stale = (it == best_priority_.end()) || (it->second != top.priority);

        if(stale){
            heap_.extract_min(); 
        } 
        else{
            break; 
        }
    }
}

int Priority_Queue::pop_min(){

    discard_stale_top();
    Heap_Item item = heap_.extract_min();
    best_priority_.erase(item.id);
    return item.id;
}

bool Priority_Queue::empty(){
    discard_stale_top();
    return heap_.empty();
}

} // namespace QomMetro::Structures