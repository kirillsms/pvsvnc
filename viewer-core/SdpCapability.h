
#ifndef _SDP_CAPABILITY_H_
#define _SDP_CAPABILITY_H_


#include "ServerMessageListener.h"
#include "CapabilitiesManager.h"
#include "viewer-core/RemoteViewerCore.h"
#include "p2p/p2ptransport.h"


class SdpCapability : public ServerMessageListener
{
public:
  SdpCapability ();
  void setCore(RemoteViewerCore * m_core, P2pTransport * p2p);
  virtual ~SdpCapability ();
  
  virtual void onServerMessage(UINT32 code, DataInputStream *input);
  virtual void addCapabilities(CapabilitiesManager *capabilitiesManager);

private:
	RemoteViewerCore * m_core;
	P2pTransport * m_p2p;

};

#endif
