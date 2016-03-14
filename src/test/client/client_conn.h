/*
 * client.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __TEST_CLIENT_CONN_H__
#define __TEST_CLIENT_CONN_H__

#include "pkt_definition.h"
#include "util.h"
#include <string>
using std::string;

class ClientConn {
public:
    ClientConn();
    virtual ~ClientConn();
    
    bool IsConnected();
    int Connect(const string& server_ip, uint16_t server_port, uint32_t timeout);
    int Reconnect();
    int Close();
    
    int SendPacket(PktBase* pPkt);
    PktBase* RecvPacket();
    
private:
    string          server_ip_;
    uint16_t        server_port_;
    uint32_t        timeout_;
    
    net_handle_t    conn_handle_;
    uchar_t*        in_buf_;
    int             alloc_size_;
};

#endif
