#pragma once
#include "SocketIPv4.h"
class KonturSocketIPv4 :
	public SocketIPv4
{
public:
	void connect(const TCHAR *host, unsigned short port) throw(SocketException);
	KonturSocketIPv4(void);
	virtual ~KonturSocketIPv4(void);
};

