/*
 * mai.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

/*
    use ./client to presure test TCP server
    use ab -n 10000 -c 10 to presure test HTTP server
 */
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include "event_loop.h"
#include "base_conn.h"
#include "serv_conn.h"
#include "http_conn.h"
#include "http_handler_map.h"
#include "http_parser_wrapper.h"
#include "simple_log.h"

#define TEST_URL    "/test"

struct Config {
    string  host;
    int     port;
    int     http_port;
    int     io_thread_number;
    int     work_thread_number;
};

Config g_config;

void init_default_config()
{
    g_config.host = "127.0.0.1";
    g_config.port = 8888;
    g_config.http_port = 8008;
    g_config.io_thread_number = 0;
    g_config.work_thread_number = 1;
}

void print_usage(const char* program)
{
    fprintf(stderr, "%s [OPTIONS]\n"
            "  --host <host>        server ip (default: 127.0.0.1)\n"
            "  --port <port>        server port (default: 8888)\n"
            "  --http-port <port>   http server port (default: 8008)\n"
            "  --io-thread <io_thread-number> concurrent io thread number(default: 0)\n"
            "  --work-thread <worker-thread-number> concurrent work thread number(default: 1)\n"
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
        } else if (!strcmp(argv[i], "--http-port") && !last_arg) {
            g_config.http_port = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--io-thread") && !last_arg) {
            g_config.io_thread_number = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "--work-thread") && !last_arg) {
            g_config.work_thread_number = atoi(argv[++i]);
        } else {
            print_usage(argv[0]);
            exit(1);
        }
    }
}

void handle_http_test(HttpConn* pHttpConn, HttpParserWrapper* pHttpParser)
{
    printf("url=%s, content=%s\n", pHttpParser->GetUrl(), pHttpParser->GetBodyContent());
    
    string content = "<h2>Hello World</h2>\n";
    char http_buf[10240];
    snprintf(http_buf, sizeof(http_buf), HTTP_HEADER, (int)content.size(), content.c_str());
    pHttpConn->Send(http_buf, (int)strlen(http_buf));
    pHttpConn->Close();
}

int main(int argc, char* argv[])
{
    long cpu_num = sysconf(_SC_NPROCESSORS_CONF);
    printf("cpu_num=%ld\n", cpu_num);
    
    init_default_config();
    parse_cmd_line(argc, argv);
    
    init_thread_event_loops(g_config.io_thread_number);
    init_thread_base_conn(g_config.io_thread_number);
    init_thread_http_conn(g_config.io_thread_number);
    
    init_server_thread_pool(g_config.work_thread_number);

    HttpHandlerMap* handler_map = HttpHandlerMap::getInstance();
    handler_map->AddHandler(TEST_URL, handle_http_test);
    
    BaseSocket tcp_socket;
    tcp_socket.Listen(g_config.host.c_str(), g_config.port, connect_callback<ServConn>, NULL);
    
    BaseSocket http_socket;
    http_socket.Listen(g_config.host.c_str(), g_config.http_port, connect_callback<HttpConn>, NULL);
    
    get_main_event_loop()->Start(1000);
    
    printf("end main eventloop\n");
    
    return 0;
}

