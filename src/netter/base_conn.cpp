/*
 * base_conn.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "base_conn.h"
#include "mutex.h"
#include "event_loop.h"
#include "io_thread_resource.h"
#include "pkt_definition.h"
#include "base_socket.h"

typedef list< pair<net_handle_t, PktBase*> > PendingPktList;

struct PendingPktMgr {
    ConnMap_t       conn_map;
    Mutex           mutex;
    PendingPktList  pkt_list;
};

IoThreadResource<PendingPktMgr> g_pending_pkt_mgr;

void loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    PendingPktMgr* pkt_mgr = (PendingPktMgr*)callback_data;
    if (!pkt_mgr)
        return;
    
    PendingPktList tmp_pkt_list;
    {
        MutexGuard mg(pkt_mgr->mutex);
        if (pkt_mgr->pkt_list.empty())
            return;
        
        tmp_pkt_list.swap(pkt_mgr->pkt_list);
    }
    
    for (PendingPktList::iterator it = tmp_pkt_list.begin(); it != tmp_pkt_list.end(); ++it) {
        ConnMap_t::iterator it_conn = pkt_mgr->conn_map.find(it->first);
        if (it_conn != pkt_mgr->conn_map.end()) {
            it_conn->second->SendPkt(it->second);
        }
        
        delete it->second;
    }
}

void conn_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    //printf("conn_callback, msg=%d, handle=%d\n", msg, handle);
    
    BaseConn* pConn = (BaseConn*)callback_data;
	if (!pConn)
		return;

    pConn->AddRef();
	switch (msg) {
        case NETLIB_MSG_CONFIRM:
            pConn->OnConfirm();
            break;
        case NETLIB_MSG_READ:
            pConn->OnRead();
            break;
        case NETLIB_MSG_WRITE:
            pConn->OnWrite();
            break;
        case NETLIB_MSG_CLOSE:
            pConn->OnClose();
            break;
        case NETLIB_MSG_TIMER:
            {
                uint64_t curr_tick = *(uint64_t*)pParam;
                pConn->OnTimer(curr_tick);
                break;
            }
        default:
            printf("!!!conn_callback error msg: %d\n", msg);
            break;
	}
    pConn->ReleaseRef();
}

void init_thread_base_conn(int io_thread_num)
{
    g_pending_pkt_mgr.Init(io_thread_num);
    
    if (io_thread_num > 0) {
        for (int i = 0; i < io_thread_num; ++i) {
            EventLoop* el = get_io_event_loop(i);
            el->AddLoop(loop_callback, g_pending_pkt_mgr.GetIOResource(i));
        }
    } else {
        get_main_event_loop()->AddLoop(loop_callback, g_pending_pkt_mgr.GetMainResource());
    }
}


//////////////////////////
BaseConn::BaseConn()
{
	//printf("BaseConn::BaseConn\n");

    m_open = false;
	m_busy = false;
	m_handle = NETLIB_INVALID_HANDLE;
    m_heartbeat_interval = kHeartBeartInterval;
    m_conn_timeout = kConnTimeout;
    
	m_last_send_tick = m_last_recv_tick = get_tick_count();
}

BaseConn::~BaseConn()
{
	//printf("BaseConn::~BaseConn, handle=%d\n", m_handle);
    
}

net_handle_t BaseConn::Connect(const string& server_ip, uint16_t server_port)
{
    m_peer_ip = server_ip;
    m_peer_port = server_port;
    
    m_base_socket = new BaseSocket();
    assert(m_base_socket);
    m_handle = m_base_socket->Connect(server_ip.c_str(), server_port, conn_callback, this);
    if (m_handle == NETLIB_INVALID_HANDLE) {
        delete this;
    }
    
    return m_handle;
}

void BaseConn::Close()
{
    if (m_handle == NETLIB_INVALID_HANDLE) {
        return;
    }
    
    if (g_pending_pkt_mgr.IsInited()) {
        PendingPktMgr* pkt_mgr = g_pending_pkt_mgr.GetIOResource(m_handle);
        pkt_mgr->conn_map.erase(m_handle);
    }
    
    printf("Client Close: handle=%d\n", m_handle);
    m_base_socket->Close();
    m_handle = NETLIB_INVALID_HANDLE;
    ReleaseRef();
}

int BaseConn::SendPkt(PktBase* pkt)
{
    return Send(pkt->GetBuffer(), pkt->GetLength());
}

int BaseConn::Send(void* data, int len)
{
	m_last_send_tick = get_tick_count();

    if (!m_open) {
        printf("connection do not open yet\n");
        return 0;
    }
    
	if (m_busy) {
		m_out_buf.Write(data, len);
		return len;
	}

	int offset = 0;
	int remain = len;
	while (remain > 0) {
		int send_size = remain;
		if (send_size > kMaxSendSize) {
			send_size = kMaxSendSize;
		}

		int ret = m_base_socket->Send((char*)data + offset , send_size);
		if (ret <= 0) {
			ret = 0;
			break;
		}

		offset += ret;
		remain -= ret;
	}

	if (remain > 0) {
		m_out_buf.Write((char*)data + offset, remain);
		m_busy = true;
		printf("send busy, remain=%d\n", m_out_buf.GetReadableLen());
	}

	return len;
}

int BaseConn::SendPkt(net_handle_t handle, PktBase* pkt)
{
    EventLoop* el = get_io_event_loop(handle);
    PendingPktMgr* pkt_mgr = g_pending_pkt_mgr.GetIOResource(handle);
    
    if (el->IsInLoopThread()) {
        ConnMap_t::iterator it_conn = pkt_mgr->conn_map.find(handle);
        if (it_conn != pkt_mgr->conn_map.end()) {
            it_conn->second->SendPkt(pkt);
        }
        
        delete pkt;
    } else {
        pkt_mgr->mutex.Lock();
        pkt_mgr->pkt_list.push_back(make_pair(handle, pkt));
        pkt_mgr->mutex.Unlock();
        
        el->Wakeup();
    }
    
    return 0;
}

void BaseConn::OnConnect(BaseSocket *base_socket)
{
    m_open = true;
    m_base_socket = base_socket;
    m_handle = base_socket->GetHandle();
    
    if (g_pending_pkt_mgr.IsInited()) {
        PendingPktMgr* pkt_mgr = g_pending_pkt_mgr.GetIOResource(m_handle);
        pkt_mgr->conn_map.insert(make_pair(m_handle, this));
    }
    
    base_socket->SetCallback(conn_callback);
    base_socket->SetCallbackData((void*)this);
    m_peer_ip = base_socket->GetRemoteIP();
    m_peer_port = base_socket->GetRemotePort();
}

void BaseConn::OnConfirm()
{
    m_open = true;
    
    if (g_pending_pkt_mgr.IsInited()) {
        PendingPktMgr* pkt_mgr = g_pending_pkt_mgr.GetIOResource(m_handle);
        pkt_mgr->conn_map.insert(make_pair(m_handle, this));
    }
}

void BaseConn::OnRead()
{
	_RecvData();

	_ParsePkt();
}

void BaseConn::OnWrite()
{
	if (!m_busy)
		return;

	while (m_out_buf.GetReadableLen() > 0) {
		int send_size = m_out_buf.GetReadableLen();
		if (send_size > kMaxSendSize) {
			send_size = kMaxSendSize;
		}

		int ret = m_base_socket->Send(m_out_buf.GetReadBuffer(), send_size);
		if (ret <= 0) {
			ret = 0;
			break;
		}

		m_out_buf.Read(NULL, ret);
	}

    m_out_buf.ResetOffset();
    
	if (m_out_buf.GetReadableLen() == 0) {
		m_busy = false;
	}

	printf("onWrite, remain=%d\n", m_out_buf.GetReadableLen());
}

void BaseConn::OnClose()
{
    Close();
}

void BaseConn::OnTimer(uint64_t curr_tick)
{
    //printf("OnTimer, curr_tick=%llu\n",curr_tick);
    if (curr_tick > m_last_recv_tick + m_conn_timeout) {
        printf("connection timeout, handle=%d\n", m_handle);
        Close();
        return;
    }
}

void BaseConn::_RecvData()
{
    for (;;) {
		uint32_t free_buf_len = m_in_buf.GetWritableLen();
		if (free_buf_len < kReadBufSize)
			m_in_buf.Extend(kReadBufSize);
        
		int ret = m_base_socket->Recv(m_in_buf.GetWriteBuffer(), kReadBufSize);
		if (ret <= 0)
			break;
        
		m_in_buf.IncWriteOffset(ret);
		m_last_recv_tick = get_tick_count();
	}
}

void BaseConn::_ParsePkt()
{
    try {
		PktBase* pPkt = NULL;
		while ( (pPkt = PktBase::ReadPacket(m_in_buf.GetReadBuffer(), m_in_buf.GetReadableLen())) ) {
			uint32_t pkt_len = pPkt->GetLength();

			HandlePkt(pPkt);
            
			m_in_buf.Read(NULL, pkt_len);
			delete pPkt;
            
            if (m_handle == NETLIB_INVALID_HANDLE) {
                // 有可能处理数据包后就关闭了连接, 再处理数据包会crash，所以这里要这么判断下
                break;
            }
		}
        
        m_in_buf.ResetOffset();
	} catch (PktException& ex) {
        if (ex.GetErrorCode() == ERROR_CODE_WRONG_PKT_LEN) {
            printf("!!!exception, err_msg=%s\n", ex.GetErrorMsg());
        } else {
            // 读取包头来确认pkt_id
            pkt_header_t header;
            PktBase::ReadPktHeader(m_in_buf.GetReadBuffer(), PKT_HEADER_LEN, &header);
            printf("!!!exception: pkt_len=%d, pkt_id=%d, err_msg=%s\n",
                   header.length, header.pkt_id, ex.GetErrorMsg());
        }
        
        OnClose();
	}
}
