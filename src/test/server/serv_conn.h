//
//  ServConn.h
//  EventLoop
//
//  Created by ziteng on 16-3-3.
//  Copyright (c) 2016年 mgj. All rights reserved.
//

#ifndef __EventLoop__ServConn_H__
#define __EventLoop__ServConn_H__

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
