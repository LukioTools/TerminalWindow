#pragma once 

#include <iostream>
#include <vector>
#include <atomic>

namespace Queues{
template<typename param_t>
class Queue{

    public:
    std::vector<param_t> queue;   

    Queue(){
        
    }

    Queue(std::vector<param_t> arr){
        queue = arr;
    }

    void push_back(param_t object){
        queue.push_back(object);
    }

    int size(){
        queue.size();
    }

    param_t pop_front(){
        if(queue.size() > 0){
            param_t p = queue[0];
            queue.erase(queue.begin());
            return p;
        } else throw std::out_of_range("The queue is empty.");
    }
};

template<typename param_t>
class AtomicQueue {
public:
    std::vector<param_t> queue;
    std::atomic_flag lock = ATOMIC_FLAG_INIT;

    AtomicQueue() {}

    AtomicQueue(std::vector<param_t> arr) : queue(arr) {}

    void push_back(param_t object) {
        while (lock.test_and_set(std::memory_order_acquire)) {} // Spin until lock is acquired
        queue.push_back(object);
        lock.clear(std::memory_order_release); // Release the lock
    }

    int size() {
        return queue.size();
    }

    param_t get_front(){
        while (lock.test_and_set(std::memory_order_acquire)) {} // Spin until lock is acquired
        if (queue.size() > 0) {
            param_t p = queue[0];    
            lock.clear(std::memory_order_release); // Release the lock
            return p;
        }
        throw std::out_of_range("the queue is empty.");
    }

    param_t pop_front() {
        while (lock.test_and_set(std::memory_order_acquire)) {} // Spin until lock is acquired
        if (queue.size() > 0) {
            param_t p = queue[0];
            queue.erase(queue.begin());
            lock.clear(std::memory_order_release); // Release the lock
            return p;
        } else {
            lock.clear(std::memory_order_release); // Release the lock
            throw std::out_of_range("The queue is empty.");
        }
    }
};

}