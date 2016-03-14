/*
 * http_conn.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_HTTP_CONN_H__
#define __NETTER_HTTP_CONN_H__

#include "util.h"
#include "base_conn.h"

#define HTTP_HEADER "HTTP/1.1 200 OK\r\n"\
    "Cache-Control:no-cache\r\n"\
    "Connection:close\r\n"\
    "Content-Length:%d\r\n"\
    "Content-Type:text/html;charset=utf-8\r\n\r\n%s"

#define OK_CODE		1001
#define OK_MSG		"OK"

#define MAX_BUF_SIZE 819200

class HttpConn : public BaseConn {
public:
	HttpConn();
	virtual ~HttpConn();

	virtual void OnRead();                      // http数据包解析不一样，需要override
	virtual void OnTimer(uint64_t curr_tick);   // 不需要发送heartbeat，需要override
};

void init_thread_http_conn(int io_thread_num);

#endif
