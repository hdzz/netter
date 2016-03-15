/*
 * http_conn.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "http_conn.h"
#include "http_handler_map.h"
#include "http_parser_wrapper.h"
#include "io_thread_resource.h"

static HttpHandlerMap* g_http_handler_map;
IoThreadResource<HttpParserWrapper> g_http_parser_wrappers;

void init_thread_http_conn(int io_thread_num)
{
	g_http_handler_map = HttpHandlerMap::getInstance();
    g_http_parser_wrappers.Init(io_thread_num);
}

//////////////////////////
HttpConn::HttpConn()
{

}

HttpConn::~HttpConn()
{

}

void HttpConn::OnRead()
{
	_RecvData();

    // 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
    char* in_buf = (char*)m_in_buf.GetBuffer();
    uint32_t buf_len = m_in_buf.GetWriteOffset();
    in_buf[buf_len] = '\0';
    
    HttpParserWrapper* parser_wrapper = g_http_parser_wrappers.GetIOResource(m_handle);
    parser_wrapper->ParseHttpContent(in_buf, buf_len);
    
    if (parser_wrapper->IsReadAll()) {
        string handler_url;     // URL里面'?'前面部分内容，没有'?'则是整个URL
        char* url =  parser_wrapper->GetUrl();
        
        char* url_end = strchr(url, '?');
        if (url_end) {
            handler_url.append(url, url_end - url);
        } else {
            handler_url = url;
        }
        
        http_handler_t handler = g_http_handler_map->GetHandler(handler_url);
        if (handler) {
          handler(this, parser_wrapper);
        } else {
            printf("no handler for: %s\n", url);
            string resp_str = "{\"code\": 404, \"msg\": \"no such method\"}\n";
            char buf[1024];
            snprintf(buf, 1024, HTTP_HEADER, (int)resp_str.size(), resp_str.c_str());
            Send(buf, (int)strlen(buf));
            Close();
        }
    }
}

