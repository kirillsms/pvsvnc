#include "KonturSocketIPv4.h"
#include <Mstcpip.h>


KonturSocketIPv4::KonturSocketIPv4(void)
	:SocketIPv4()
{
	const int one = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&one, sizeof(one));
	setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&one, sizeof(one));
	
	tcp_keepalive keepalive_requested, keepalive_returned;

	DWORD bytes_returned = 0;
	keepalive_requested.onoff = 1;
	keepalive_requested.keepalivetime = 5000;
	keepalive_requested.keepaliveinterval = 1000;

	WSAIoctl(m_socket, SIO_KEEPALIVE_VALS, 
					&keepalive_requested, sizeof(keepalive_requested), 
					&keepalive_returned, sizeof(keepalive_returned), 
					&bytes_returned, NULL, NULL);
}


KonturSocketIPv4::~KonturSocketIPv4(void)
{
}

void KonturSocketIPv4::connect(const TCHAR *host, unsigned short port)
{
	SocketIPv4::connect(host, port);
}