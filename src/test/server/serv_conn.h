/*
 * serv_conn.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __TEST_SERV_CONN_H__
#define __TEST_SERV_CONN_H__

#include "base_conn.h"

class ServConn : public BaseConn {
public:
    ServConn();
    virtual ~ServConn();
    
    virtual void OnConnect(BaseSocket* base_socket);
	virtual void HandlePkt(PktBase* pPkt);
};

void init_server_thread_pool(int worker_thread_num);

#endif
