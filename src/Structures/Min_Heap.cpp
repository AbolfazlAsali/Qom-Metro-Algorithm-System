#include "../../include/Structures/Min_Heap.hpp"

#include <algorithm>
#include <cassert>

using namespace std;


namespace QomMetro::Structures {

size_t Min_Heap::parent(size_t index) const{
    return (index - 1) / 2;
}

size_t Min_Heap::left_Child(size_t index) const{
    return 2 * index + 1;
}

size_t Min_Heap::right_Child(size_t index) const{
    return 2 * index + 2;
}

void Min_Heap::push(double priority, int id){

    data_.push_back(Heap_Item{priority, id});
    heapUp(data_.size() - 1);
}

void Min_Heap::heapUp(size_t index){

    while(index > 0){

        size_t parent_ = parent(index);
        if(data_[index].priority < data_[parent_].priority){

            swap(data_[index], data_[parent_]);
            index = parent_;
        } 
        else{
            break;
        }
    }
}

Heap_Item Min_Heap::extract_min(){

    assert(!data_.empty() && "extract_min() called on an empty Min_Heap");

    Heap_Item min_item = data_[0];
    data_[0] = data_.back();
    data_.pop_back();

    if(!data_.empty()){
        heapDown(0);
    }
    return min_item;
}

void Min_Heap::heapDown(size_t index){

    size_t size = data_.size();
    while(true){

        size_t left = left_Child(index);
        size_t right = right_Child(index);
        size_t smallest = index;

        if(left < size && data_[left].priority < data_[smallest].priority){
            smallest = left;
        }
        if(right < size && data_[right].priority < data_[smallest].priority){
            smallest = right;
        }
        if(smallest == index){
            break;
        }
        swap(data_[index], data_[smallest]);
        index = smallest;
    }
}

const Heap_Item& Min_Heap::peek() const{
    assert(!data_.empty() && "peek() called on an empty Min_Heap");
    return data_[0];
}

bool Min_Heap::empty() const{
    return data_.empty();
}

size_t Min_Heap::size() const{
    return data_.size();
}
} // namespace QomMetro::Structures