
#ifndef __SDPHANDLER_H__
#define __SDPHANDLER_H__

#include "RfbDispatcherListener.h"
#include "RfbCodeRegistrator.h"

#include "network/RfbInputGate.h"
#include "network/RfbOutputGate.h"
#include "log-writer/LogWriter.h"

#include "thread/AutoLock.h"

#include "p2p/p2ptransport.h"



class SdpHandler : public RfbDispatcherListener

{
public:
  SdpHandler(RfbCodeRegistrator *registrator,
                             RfbOutputGate *output,
                             LogWriter *log, P2pTransport * p2p);
  virtual ~SdpHandler();
    void sendSDPToClient();

protected:

  virtual void onRequest(UINT32 reqCode, RfbInputGate *input);
  RfbOutputGate *m_output;
  LogWriter *m_log;
  P2pTransport * m_p2p;
};

#endif // __SDPHANDLER_H__
