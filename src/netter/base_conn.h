/*
 * base_conn.h
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#ifndef __NETTER_BASE_CONN_H_
#define __NETTER_BASE_CONN_H_

#include "util.h"
#include "simple_buffer.h"
#include "base_socket.h"

const int kHeartBeartInterval =	2000;    // 服务端心跳间隔-2s
const int kConnTimeout = 5000;   // 服务端超时-5s

const int kMaxSendSize = 128 * 1024;
const int kReadBufSize = 2048;

class PktBase;

class BaseConn : public RefCount
{
public:
	BaseConn();
	virtual ~BaseConn();

    int SendPkt(PktBase* pkt);    // 需要发送者自己delete pkt
    int Send(void* data, int len);

    bool IsOpen() { return m_open; }
    void SetHeartbeatInterval(int interval) { m_heartbeat_interval = interval; }
    void SetConnTimerout(int timeout) { m_conn_timeout = timeout; }
    net_handle_t GetHandle() { return m_handle; }
    char* GetPeerIP() { return (char*)m_peer_ip.c_str(); }
    uint16_t GetPeerPort() { return m_peer_port; }
    void AddToWaitPktList(PktBase* pkt) { m_wait_pkt_list.push_back(pkt); }
    
    virtual net_handle_t Connect(const string& server_ip, uint16_t server_port);
    virtual void Close();
    
	virtual void OnConnect(BaseSocket* base_socket);
	virtual void OnConfirm();
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePkt(PktBase* pPkt) {}
  
    // pkt需要是一个new出来的包，而且发送者不能delete该数据包，有网络库删除
    static int SendPkt(net_handle_t handle, PktBase* pkt);
    static int CloseHandle(net_handle_t handle);
protected:
    void _RecvData();
    void _ParsePkt();
    
protected:
    BaseSocket*     m_base_socket;
	net_handle_t	m_handle;
	bool			m_busy;
    bool            m_open;
    int             m_heartbeat_interval;
    int             m_conn_timeout;
    
	string			m_peer_ip;
	uint16_t		m_peer_port;
	SimpleBuffer	m_in_buf;
	SimpleBuffer	m_out_buf;
    list<PktBase*>  m_wait_pkt_list;    // 到服务端连接连接前，等待发送的数据包列表

	uint64_t		m_last_send_tick;
	uint64_t		m_last_recv_tick;
};

void init_thread_base_conn(int io_thread_num);

typedef unordered_map<net_handle_t, BaseConn*> ConnMap_t;

#endif /* __BASE_CONN_H_ */
