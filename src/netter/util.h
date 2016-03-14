/*
 * util.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_UTIL_H__
#define __NETTER_UTIL_H__

#include "ostype.h"
#include "mutex.h"
#include <string.h>
#include <strings.h>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
using namespace std;

#define NOTUSED_ARG(v) ((void)v)		// used this to remove warning C4100, unreferenced parameter

enum {
    ERROR_CODE_WRONG_PKT_LEN        = 1,
    ERROR_CODE_PARSE_FAILED         = 2,
    ERROR_CODE_UNKNOWN_PKT_ID       = 3,
};

class  PktException {
public:
	PktException(uint32_t error_code, const char* error_msg) {
		error_code_ = error_code;
		error_msg_ = error_msg;
	}
	virtual ~PktException() {}
    
	uint32_t GetErrorCode() { return error_code_; }
	char* GetErrorMsg() { return (char*)error_msg_.c_str(); }
private:
	uint32_t	error_code_;
	string		error_msg_;
};

class RefCount
{
public:
	RefCount() : ref_count_(1), mutex_(NULL) {}
	virtual ~RefCount() {}
    
	void SetMutex(Mutex* mtx) { mutex_ = mtx; }

	void AddRef() {
        if (mutex_) {
            mutex_->Lock();
            ++ref_count_;
            mutex_->Unlock();
        } else {
            ++ref_count_;
        }
    }
    
	void ReleaseRef() {
        if (mutex_) {
            MutexGuard mg(*mutex_);
            --ref_count_;
            if (ref_count_ == 0) {
                delete this;
            }
        } else {
            --ref_count_;
            if (ref_count_ == 0)
                delete this;
        }
    }
private:
	int		ref_count_;
	Mutex*	mutex_;
};

uint64_t get_tick_count();

#endif
