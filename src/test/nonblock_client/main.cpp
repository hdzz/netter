/*
 * main.cpp
 *
 *  Created on: 2016-3-15
 *      Author: ziteng
 */

#include <stdio.h>
#include <stdlib.h>
#include "base_conn.h"
#include "nb_client_conn.h"
#include "event_loop.h"
#include "pkt_definition.h"

struct Config {
    string  host;
    int     port;
    int     thread_number;
};

Config g_config;

void init_default_config()
{
    g_config.host = "127.0.0.1";
    g_config.port = 8888;
    g_config.thread_number = 1;
}

void print_usage(const char* program)
{
    fprintf(stderr, "%s [OPTIONS]\n"
            "  --host <host>            server ip (default: 127.0.0.1)\n"
            "  --port <port>            server port (default: 8888)\n"
            "  --thread <thread-number> concurrent thread number to execute the client (default: 1)\n"
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
        } else if (!strcmp(argv[i], "--thread") && !last_arg) {
            g_config.thread_number = atoi(argv[++i]);
        } else {
            print_usage(argv[0]);
            exit(1);
        }
    }
}

int main(int argc, char* argv[])
{
    init_default_config();
    parse_cmd_line(argc, argv);
    
    init_thread_event_loops(g_config.thread_number);
    init_thread_base_conn(g_config.thread_number);
    
    for (int i = 0; i < g_config.thread_number; ++i) {
        NbClientConn* client = new NbClientConn();
        net_handle_t handle = client->Connect(g_config.host, g_config.port);
        
        if (handle != NETLIB_INVALID_HANDLE) {
            PktHeartBeat* pkt = new PktHeartBeat;
            BaseConn::SendPkt(handle, pkt);
        }
    }
    
    get_main_event_loop()->Start();
    
    return 0;
}