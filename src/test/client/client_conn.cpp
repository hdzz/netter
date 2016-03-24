/*
 * client_conn.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include <assert.h>
#include "client_conn.h"
#include "block_socket.h"

const int kMaxTimeout = 500;    // 500ms
const int kInitAllocSize = 4096;

ClientConn::ClientConn()
{
    server_port_ = 0;
    timeout_ = kMaxTimeout;
    conn_handle_ = NETLIB_INVALID_HANDLE;
    in_buf_ = (uchar_t*)malloc(kInitAllocSize);
    assert(in_buf_ != NULL);
    alloc_size_ = kInitAllocSize;
}

ClientConn::~ClientConn()
{
    free(in_buf_);
    if (conn_handle_ != NETLIB_INVALID_HANDLE) {
        Close();
    }
}

bool ClientConn::IsConnected()
{
    return !is_socket_closed(conn_handle_);
}

int ClientConn::Connect(const string& server_ip, uint16_t server_port, uint32_t timeout)
{
    server_ip_ = server_ip;
    server_port_ = server_port;
    timeout_ = timeout;
    conn_handle_ = connect_with_timeout(server_ip.c_str(), server_port, timeout);
    if (conn_handle_ == NETLIB_INVALID_HANDLE) {
        return 1;
    } else {
        block_set_timeout(conn_handle_, timeout);
        return 0;
    }
}

int ClientConn::Reconnect()
{
    if (conn_handle_ != NETLIB_INVALID_HANDLE) {
        Close();
    }
    
    conn_handle_ = connect_with_timeout(server_ip_.c_str(), server_port_, timeout_);
    if (conn_handle_ == NETLIB_INVALID_HANDLE) {
        return 1;
    } else {
        block_set_timeout(conn_handle_, timeout_);
        return 0;
    }
}

int ClientConn::Close()
{
    block_close(conn_handle_);
    conn_handle_ = NETLIB_INVALID_HANDLE;
    return 0;
}

int ClientConn::SendPacket(PktBase* pPkt)
{
    if (conn_handle_ == NETLIB_INVALID_HANDLE) {
        return 0;
    }
    
    if (is_socket_closed(conn_handle_)) {
        int ret = Reconnect();
        if (ret) {
            return 0;
        }
    }
    
    return block_send_all(conn_handle_, pPkt->GetBuffer(), pPkt->GetLength());
}

PktBase* ClientConn::RecvPacket()
{
    if (conn_handle_ == NETLIB_INVALID_HANDLE) {
        printf("invaid connectiont %s:%d\n", server_ip_.c_str(), server_port_);
        return NULL;
    }
    
    // read header
    int len = block_recv_all(conn_handle_, in_buf_, PKT_HEADER_LEN);
    if (len != PKT_HEADER_LEN) {
        printf("read packet header failed\n");
        return NULL;
    }
    
    int pkt_len = (int)CByteStream::ReadUint32(in_buf_);
    if (pkt_len > PKT_HEADER_LEN) {
        // read body
        if (pkt_len > alloc_size_) {
            in_buf_ = (uchar_t*)realloc(in_buf_, pkt_len);
            alloc_size_ = pkt_len;
            assert(in_buf_ != NULL);
        }
        
        len = block_recv_all(conn_handle_, in_buf_ + PKT_HEADER_LEN, pkt_len - PKT_HEADER_LEN);
        if (len != (pkt_len - PKT_HEADER_LEN)) {
            printf("read packet body failed, %d(%d)\n", pkt_len - PKT_HEADER_LEN, len);
            return NULL;
        }
    }
    
    PktBase* pkt = NULL;
    try {
        pkt = PktBase::ReadPacket(in_buf_, pkt_len);
    } catch (PktException& ex) {
        printf("ReadPacket failed: %s\n", ex.GetErrorMsg());
    }
    
    return pkt;
}
