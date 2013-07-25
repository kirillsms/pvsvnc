#pragma once
#include "RfbClient.h"
class KonturRfbClient :
	public RfbClient
{
public:
	KonturRfbClient(NewConnectionEvents *newConnectionEvents, SocketIPv4 *socket,
		            ClientTerminationListener *extTermListener,
		            ClientAuthListener *extAuthListener, bool viewOnly,
		            bool isOutgoing, unsigned int id,
		            const ViewPortState *constViewPort,
		            const ViewPortState *dynViewPort,
		            LogWriter *log);




	virtual ~KonturRfbClient(void);


};

