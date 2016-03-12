//
//  simple_log.h
//  EventLoop
//
//  Created by ziteng on 16-3-11.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#ifndef __EventLoop__simple_log_H__
#define __EventLoop__simple_log_H__

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "mutex.h"
using std::string;

enum LogLevel {
    kLogLevelError      = 0,
    kLogLevelWarning    = 1,
    kLogLevelInfo       = 2,
    kLogLevelDebug      = 3,
};

const string kLogLevelName[] = {"[ERROR]", "[WARNING]", "[INFO]", "[DEBUG]"};

const int kMaxLogLineSize = 4096;

class SimpleLog {
public:
    SimpleLog();
    virtual ~SimpleLog();
    
    void Init(LogLevel level, string path);
    
    void LogMessage(LogLevel level, const char* fmt, ...);
    
private:
    void OpenFile();
    bool IsNewDay(time_t current_time);
private:
    string      log_path_;
    FILE*       log_file_;
    time_t      last_log_time_;
    LogLevel    current_log_level_;
    char        log_buf_[kMaxLogLineSize];
    Mutex       mutex_;
};

#endif
