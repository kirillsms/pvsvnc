#pragma once
#include "RfbInitializer.h"
class KonturRfbInitializer :
	public RfbInitializer
{
public:
	KonturRfbInitializer(Channel *stream,
		                 ClientAuthListener *extAuthListener,
		                 RfbClient *client, bool authAllowed);


	virtual ~KonturRfbInitializer(void);
protected:
	virtual void initVersion();
};

