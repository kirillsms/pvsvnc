#include "KonturRfbClientManager.h"
#include "rfb-sconn/KonturRfbClient.h"


KonturRfbClientManager::KonturRfbClientManager(const TCHAR *serverName,
							                   NewConnectionEvents *newConnectionEvents,
							                   LogWriter *log,
							                   DesktopFactory *desktopFactory)
:RfbClientManager(serverName, newConnectionEvents, log, desktopFactory)
{}

KonturRfbClientManager::~KonturRfbClientManager(void)
{
}

void KonturRfbClientManager::addNewKonturConnection(SocketIPv4 *socket, ViewPortState *constViewPort,
                        bool viewOnly, bool isOutgoing)
{
  AutoLock al(&m_clientListLocker);

  _ASSERT(constViewPort != 0);
  m_nonAuthClientList.push_back(new KonturRfbClient(m_newConnectionEvents,
                                              socket, this, this, viewOnly,
                                              isOutgoing,
                                              m_nextClientId,
                                              constViewPort,
                                              &m_dynViewPort,
                                              m_log));
  m_nextClientId++;
}