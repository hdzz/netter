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

template <class T>
void connect_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT) {
		T* pConn = new T();
		pConn->OnConnect((BaseSocket*)pParam);
	} else {
		printf("!!!error msg: %d\n", msg);
	}
}

class BaseConn
{
public:
	BaseConn();
	virtual ~BaseConn();

    virtual net_handle_t Connect(const string& server_ip, uint16_t server_port);
    virtual void Close();
    int SendPkt(PktBase* pkt);    // 需要发送者自己delete pkt
    int Send(void* data, int len);

    void SetHeartbeatInterval(int interval) { m_heartbeat_interval = interval; }
    void SetConnTimerout(int timeout) { m_conn_timeout = timeout; }
    net_handle_t GetHandle() { return m_handle; }
    char* GetPeerIP() { return (char*)m_peer_ip.c_str(); }
    uint16_t GetPeerPort() { return m_peer_port; }
    
	virtual void OnConnect(BaseSocket* base_socket);
	virtual void OnConfirm();
	virtual void OnRead();
	virtual void OnWrite();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePkt(PktBase* pPkt) {}
  
    // pkt需要是一个new出来的包，而且发送者不能delete该数据包，有网络库删除
    static int SendPkt(net_handle_t handle, PktBase* pkt);
protected:
    void _RecvData();
    void _ParseWholePkt();
    
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

	uint64_t		m_last_send_tick;
	uint64_t		m_last_recv_tick;
};

void init_thread_base_conn(int io_thread_num);

typedef unordered_map<net_handle_t, BaseConn*> ConnMap_t;

#endif /* __BASE_CONN_H_ */
