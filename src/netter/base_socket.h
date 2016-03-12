/*
 *  a wrap for non-block socket class for Windows, LINUX and MacOS X platform
 */

#ifndef __BASE_SOCKET_H__
#define __BASE_SOCKET_H__

#include <string>
#include "ostype.h"
using std::string;

enum
{
	SOCKET_STATE_IDLE,
	SOCKET_STATE_LISTENING,
	SOCKET_STATE_CONNECTING,
	SOCKET_STATE_CONNECTED,
	SOCKET_STATE_CLOSING
};

class EventLoop;

class BaseSocket
{
public:
	BaseSocket();
	virtual ~BaseSocket();

	net_handle_t GetHandle() { return m_handle; }
	int GetSocket() { return m_socket; }
    void SetEventLoop(EventLoop* el) { m_event_loop = el; }
	void SetSocket(int fd) { m_socket = fd; }
	void SetState(uint8_t state) { m_state = state; }
	void SetCallback(callback_t callback) { m_callback = callback; }
	void SetCallbackData(void* data) { m_callback_data = data; }
	void SetRemoteIP(char* ip) { m_remote_ip = ip; }
	void SetRemotePort(uint16_t port) { m_remote_port = port; }

    EventLoop*  GetEventLoop() { return m_event_loop; }
	const char*	GetRemoteIP() { return m_remote_ip.c_str(); }
	uint16_t	GetRemotePort() { return m_remote_port; }
	const char*	GetLocalIP() { return m_local_ip.c_str(); }
	uint16_t	GetLocalPort() { return m_local_port; }
public:
	net_handle_t Listen(
		const char*		server_ip, 
		uint16_t		port,
		callback_t		callback,
		void*			callback_data);

	net_handle_t Connect(
		const char*		server_ip, 
		uint16_t		port,
		callback_t		callback,
		void*			callback_data,
		const char* 	local_ip = NULL,
		uint16_t		local_port = 0);

	int Send(void* buf, int len);

	int Recv(void* buf, int len);

	int Close();

public:
    void OnConnect();
	void OnRead();
	void OnWrite();
	void OnClose();
    void OnTimer(uint64_t curr_tick);

private:	
	int _GetErrorCode();
	bool _IsBlock(int error_code);

	void _SetNonblock(int fd);
	void _SetReuseAddr(int fd);
	void _SetNoDelay(int fd);
	void _SetAddr(const char* ip, const uint16_t port, sockaddr_in* pAddr);
    void _GetBindAddr();

	void _AcceptNewSocket();

private:
    EventLoop*      m_event_loop;
	string			m_remote_ip;
	uint16_t		m_remote_port;
	string			m_local_ip;
	uint16_t		m_local_port;

	callback_t		m_callback;
	void*			m_callback_data;

	uint8_t			m_state;
	int             m_socket;
	net_handle_t	m_handle;
};

BaseSocket* FindBaseSocket(net_handle_t handle);

#endif
