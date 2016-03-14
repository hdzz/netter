/*
 * main.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "util.h"
#include "mutex.h"
#include "thread_pool.h"
#include "client_conn.h"

Mutex g_print_mutex;

struct Config {
    string  host;
    int     port;
    int     timeout;
    int     thread_number;
    int     total_query_count;
};

Config g_config;

void init_default_config()
{
    g_config.host = "127.0.0.1";
    g_config.port = 8888;
    g_config.timeout = 500;
    g_config.thread_number = 1;
    g_config.total_query_count = 100000;
}

void print_usage(const char* program)
{
    fprintf(stderr, "%s [OPTIONS]\n"
            "  --host <host>            server ip (default: 127.0.0.1)\n"
            "  --port <port>            server port (default: 8888)\n"
            "  --timeout  <timeout>     command execution timeout in milliseconds (default: 500)\n"
            "  --thread <thread-number> concurrent thread number to execute the client (default: 1)\n"
            "  --total-query <count>    toatl query count in one thread (default: 100000)\n"
            "  --help\n", program);
}

void parse_cmd_line(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        bool last_arg = (i == argc - 1);
        
        if (!strcmp(argv[i], "--help")) {
            print_usage(argv[0]);
            exit(0);
        } else if (!strcmp(argv[i], "--host") && !last_arg) {
            g_config.host = argv[++i];
        } else if (!strcmp(argv[i], "--port") && !last_arg) {
            g_config.port = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--timeout") && !last_arg) {
            g_config.timeout = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--thread") && !last_arg) {
            g_config.thread_number = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--total-query") && !last_arg) {
            g_config.total_query_count = atoi(argv[++i]);
        } else {
            print_usage(argv[0]);
            exit(1);
        }
    }
}

class PressureTestThread : public Thread {
public:
    pthread_t GetThreadId() { return m_thread_id; }
    
    void PrintResult(const string& cmd_name, uint64_t cost_tick, map<uint64_t, int>& histogram) {
        MutexGuard mg(g_print_mutex);
        fprintf(stderr, "(thread_id=%ld) %d %s takes %d ms\n",
                (long)m_thread_id, g_config.total_query_count, cmd_name.c_str(), (int)cost_tick);
        for (map<uint64_t, int>::iterator it = histogram.begin(); it != histogram.end(); ++it) {
            fprintf(stderr, "%d ms = %d\n", (int)it->first, it->second);
        }
        fprintf(stderr, "\n");
    }
    
    virtual void OnThreadRun() {
        ClientConn conn;
        int ret = conn.Connect(g_config.host, g_config.port, 100);
        if (ret != 0) {
            printf("connect to %s:%d failed\n", g_config.host.c_str(), g_config.port);
            return;
        } else {
            printf("connect to %s:%d sucsess\n", g_config.host.c_str(), g_config.port);
        }
        
        map<uint64_t, int> histogram;
        PktHeartBeat pkt;
        uint64_t start_tick = get_tick_count();
        for (int i = 0; i < g_config.total_query_count; ++i) {
            uint64_t s_tick = get_tick_count();
            pkt.SetSeqNo(i + 1);
            conn.SendPacket(&pkt);
            PktBase* pPkt = conn.RecvPacket();
            if (!pPkt) {
                printf("receive packet failed\n");
            } else {
                delete pPkt;
            }
            uint64_t c_tick = get_tick_count() - s_tick;
            
            ++histogram[c_tick];
        }
        uint64_t cost_tick = get_tick_count() - start_tick;
        
        PrintResult("heartbeat", cost_tick, histogram);
    }
};

int main(int argc, char* argv[])
{
    init_default_config();
    parse_cmd_line(argc, argv);
    
    PressureTestThread* threads = new PressureTestThread[g_config.thread_number];
    for (int i = 0; i < g_config.thread_number; ++i) {
        threads[i].StartThread();
    }
    
    for (int i = 0; i < g_config.thread_number; ++i) {
        pthread_join(threads[i].GetThreadId(), NULL);
    }
    
    return 0;
}
