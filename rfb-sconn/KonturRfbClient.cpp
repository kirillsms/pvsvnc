#include "KonturRfbClient.h"
#include "KonturRfbInitializer.h"
#include "thread/AutoLock.h"
#include "RfbCodeRegistrator.h"
#include "ft-server-lib/FileTransferRequestHandler.h"
#include "network/socket/SocketStream.h"
#include "RfbInitializer.h"
#include "ClientAuthListener.h"
#include "server-config-lib/Configurator.h"
#include "tvnserver-app\TvnServer.h"

KonturRfbClient::KonturRfbClient(NewConnectionEvents *newConnectionEvents, SocketIPv4 *socket,
				 	            ClientTerminationListener *extTermListener,
				 	            ClientAuthListener *extAuthListener, bool viewOnly,
				 	            bool isOutgoing, unsigned int id,
				 	            const ViewPortState *constViewPort,
				 	            const ViewPortState *dynViewPort,
				 	            LogWriter *log)

:RfbClient(newConnectionEvents, socket, extTermListener, extAuthListener, viewOnly, isOutgoing, id, constViewPort, dynViewPort, log)






{
	m_rfbInitializer = new KonturRfbInitializer(m_sockStream, m_extAuthListener, this,
                                !m_isOutgoing);
}


KonturRfbClient::~KonturRfbClient(void)
{
  
  TvnServer * srv = TvnServer::getInstance();
  sleep(2000);
  srv->setRepeaterStatus(new StringStorage(_T(":Reconnect")));
  srv->startRepeaterOutgoingConnection();

}


