//
//  mutex.h
//  EventLoop
//
//  Created by ziteng on 16-3-2.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#ifndef __EventLoop__mutex_H__
#define __EventLoop__mutex_H__

#include <pthread.h>

class Mutex {
public:
    Mutex() { pthread_mutex_init(&mutex_, NULL); }
    ~Mutex() { pthread_mutex_destroy(&mutex_); }
    void Lock() { pthread_mutex_lock(&mutex_); }
    void Unlock() { pthread_mutex_unlock(&mutex_); }
private:
    pthread_mutex_t mutex_;
};

class MutexGuard {
public:
    MutexGuard(Mutex& m) :mutex_(m) { mutex_.Lock(); }
    ~MutexGuard() { mutex_.Unlock(); }
private:
    Mutex&   mutex_;
};

#endif
