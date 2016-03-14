/*
 * mutex.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_MUTEX_H__
#define __NETTER_MUTEX_H__

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

class RwMutex {
public:
    RwMutex() { pthread_rwlock_init(&rw_mutex_, NULL); }
    ~RwMutex() { pthread_rwlock_destroy(&rw_mutex_); }
    
    void RdLock() { pthread_rwlock_rdlock(&rw_mutex_); }
    void WrLock() { pthread_rwlock_wrlock(&rw_mutex_); }
    void Unlock() { pthread_rwlock_unlock(&rw_mutex_); }
private:
    pthread_rwlock_t rw_mutex_;
};

class RwMutexGuard {
public:
    RwMutexGuard(RwMutex& mtx, bool rdlock) : rw_mutex_(mtx) {
        if (rdlock) {
            rw_mutex_.RdLock();
        } else {
            rw_mutex_.WrLock();
        }
    }
    ~RwMutexGuard() { rw_mutex_.Unlock(); }
private:
    RwMutex&    rw_mutex_;
};

#endif
