#ifndef __NETLIB_H__
#define __NETLIB_H__

#include "ostype.h"

#ifdef __cplusplus
extern "C" {
#endif

net_handle_t connect_with_timeout(const char* server_ip, uint16_t port, uint32_t timeout);
    
int block_close(net_handle_t handle);
    
bool is_socket_closed(net_handle_t handle);

int block_set_timeout(net_handle_t handle, uint32_t timeout);
    
int block_send_all(net_handle_t handle, void* buf, int len);
    
int block_recv_all(net_handle_t handle, void* buf, int len);
    
#ifdef __cplusplus
}
#endif

#endif
