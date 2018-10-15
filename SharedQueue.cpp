//
// Created by Nolan Woods on 10/13/2018.
//

#include "SharedQueue.h"

template <typename T>
SharedQueue<T>::SharedQueue() : done(false) {}

template <typename T>
SharedQueue<T>::~SharedQueue() = default;

template <typename T>
T SharedQueue<T>::pop()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty())
    {
        if (done) throw Stop();
        cond_.wait(mlock, [this]{return done;});
    }
    T item = std::move(queue_.front());
    //std::swap(item, queue_.front());
    queue_.pop_front();
    return item;
}

template <typename T>
void SharedQueue<T>::push(const T &item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
void SharedQueue<T>::push(T &&item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();     // unlock before notificiation to minimize mutex con
    cond_.notify_one(); // notify one waiting thread

}

template <typename T>
void SharedQueue<T>::signal_done() {
    this->done = true;
}

template <typename T>
unsigned int SharedQueue<T>::size(bool blocking)
{
    if (blocking) {
        std::unique_lock<std::mutex> mlock(mutex_);
        int size = queue_.size();
        mlock.unlock();
        return size;
    } else {
        return queue_.size();
    }
}