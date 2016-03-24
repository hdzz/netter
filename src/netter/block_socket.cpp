/*
 * block_socket.cpp
 *
 *  Created on: 2016-3-14
 *      Author: ziteng
 */

#include "block_socket.h"
#include "base_socket.h"

const int kMaxSocketBufSize = 128 * 1024;

static int wait_connect_complete(int sock_fd, uint32_t timeout)
{
    struct timeval tv;
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock_fd, &fdset);
    
    int cnt = select(sock_fd + 1, NULL, &fdset, NULL, &tv);
    if (cnt <= 0) {
        printf("select failed or timeout: errno=%d, socket=%d\n", errno, sock_fd);
        return -1;
    }
    
    int error = 0;
    socklen_t len = sizeof(error);
    getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (error != 0) {
        printf("connecting socket error, socket=%d, error=%d\n", sock_fd, error);
        return -1;
    }
    
    return 0;
}

net_handle_t connect_with_timeout(const char* server_ip, uint16_t port, uint32_t timeout)
{
    printf("connect_with_timeout, server_addr=%s:%d, timeout=%d\n",server_ip, port, timeout);
    
	net_handle_t sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == INVALID_SOCKET) {
		printf("socket failed, err_code=%d\n", errno);
		return NETLIB_INVALID_HANDLE;
	}
    
    BaseSocket::SetNoDelay(sock_fd);
    BaseSocket::SetNonblock(sock_fd, true);
    
	sockaddr_in serv_addr;
    BaseSocket::SetAddr(server_ip, port, &serv_addr);
    
    int ret = connect(sock_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
	if (ret == -1) {
        if ((errno != EINPROGRESS) || wait_connect_complete(sock_fd, timeout)) {
            printf("connect failed, err_code=%d\n", errno);
            close(sock_fd);
            return NETLIB_INVALID_HANDLE;
        }
	}
	
    BaseSocket::SetNonblock(sock_fd, false);
    
	return sock_fd;
}

int block_close(net_handle_t handle)
{
    return close(handle);
}

bool is_socket_closed(net_handle_t handle)
{
    if (handle < 0) {
        return true;
    }
    
    struct timeval tv = {0, 0};
    
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(handle, &fdset);
    
    int cnt = select(handle + 1, &fdset, NULL, NULL, &tv);
    if (cnt < 0) {
        return true;
    } else if (cnt > 0) {
        int avail = 0;
        if ( (ioctl(handle, FIONREAD, &avail) == -1) || (avail == 0)) {
            printf("peer close, socket closed\n");
            return true;
        }
    }
    
    return false;
}

int block_set_timeout(net_handle_t handle, uint32_t timeout)
{
    struct timeval tv = { timeout / 1000, ((int)timeout % 1000) * 1000};
    
    if (setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == -1) {
        printf("setsockopt(SO_RCVTIMEO) failed: %d\n", errno);
        return 1;
    }
    
    if (setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == -1) {
        printf("setsockopt(SO_SNDTIMEO) failed: %d\n", errno);
        return 1;
    }
    
    return 0;
}

int block_send_all(net_handle_t handle, void* buf, int len)
{
    if (handle == NETLIB_INVALID_HANDLE) {
        return 0;
    }
    
    int offset = 0;
	int remain = len;
	while (remain > 0) {
		int send_size = remain;
		if (send_size > kMaxSocketBufSize) {
			send_size = kMaxSocketBufSize;
		}
        
		int ret = (int)send(handle, (char*)buf + offset, send_size, 0);
		if (ret < 0) {
            printf("send failed, errno=%d\n", errno);
			break;
		} else if (ret == 0) {
            printf("send len = 0\n");
            break;
        }
        
		offset += ret;
		remain -= ret;
	}
    
    return len - remain;
}

int block_recv_all(net_handle_t handle, void* buf, int len)
{
    if (handle == NETLIB_INVALID_HANDLE) {
        return 0;
    }
    
    int received_len = 0;
    while (received_len != len) {
        int ret = (int)recv(handle, (char*)buf + received_len, len - received_len, 0);
        if (ret < 0) {
            printf("recv failed, errno=%d\n", errno);
            return -1;
        } else if (ret == 0) {
            printf("peer close\n");
            return 0;
        } else {
            received_len += ret;
        }
    }
    
    return received_len;
}
