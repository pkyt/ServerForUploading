//
//  Queue.h
//  WebServerFirst
//
//  Created by Pavlo Kytsmey on 1/20/14.
//  Copyright (c) 2014 Pavlo Kytsmey. All rights reserved.
//

#ifndef WebServerFirst_Queue_h
#define WebServerFirst_Queue_h

#include <list>
#include <pthread.h>
#include <vector>

template <typename T> class Queue{
    std::list <T> _tasks; // task list
    pthread_cond_t _cv;
    pthread_mutex_t _lock;
public:
    Queue();
    ~Queue();
    T pop();
    void push(T t);
};

template <typename T> Queue<T>::Queue(){
    pthread_cond_init(&_cv, NULL);
    pthread_mutex_init(&_lock, NULL);
}

template <typename T> Queue<T>::~Queue(){
    pthread_cond_destroy(&_cv);
    pthread_mutex_destroy(&_lock);
}

template <typename T> T Queue<T>::pop(){
    pthread_mutex_lock(&_lock); // starting working with shared variable
    while (_tasks.size() == 0){
        pthread_cond_wait(&_cv, &_lock); // waiting until new task appear
    }
    T t = _tasks.front(); // getting the first task from _task list
    _tasks.pop_front(); // erasing from list (the given task to thread)
    pthread_mutex_unlock(&_lock);
    return t;
}

template <typename T> void Queue<T>::push(T t){
    pthread_mutex_lock(&_lock); // starting working with shared variable
    _tasks.push_back(t); // adding new Taks into _tasks list at the back
    pthread_cond_signal(&_cv); // signaling that new Task appeared
    pthread_mutex_unlock(&_lock);
}

#endif
