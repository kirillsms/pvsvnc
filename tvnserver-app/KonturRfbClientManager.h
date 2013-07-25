#pragma once
#include "RfbClientManager.h"
class KonturRfbClientManager :
	public RfbClientManager
{
public:
	KonturRfbClientManager(const TCHAR *serverName,
		                   NewConnectionEvents *newConnectionEvents,
		                   LogWriter *log,
		                   DesktopFactory *desktopFactory);




	virtual ~KonturRfbClientManager(void);

	void addNewKonturConnection(SocketIPv4 *socket, ViewPortState *constViewPort,
                        bool viewOnly, bool isOutgoing);
};

