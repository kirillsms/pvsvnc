#include "KonturRfbInitializer.h"
#include "util/AnsiStringStorage.h"
#include "tvnserver-app/TvnServer.h"


KonturRfbInitializer::KonturRfbInitializer(Channel *stream,
		                 ClientAuthListener *extAuthListener,
		                 RfbClient *client, bool authAllowed)
						 : RfbInitializer(stream, extAuthListener, client, authAllowed)



{
}


KonturRfbInitializer::~KonturRfbInitializer(void)
{
}

void KonturRfbInitializer::initVersion()
{
  
  char serverProtocol[13];
  char repeaterHandshake[250];
  serverProtocol[12] = 0;
  TvnServer * srv = TvnServer::getInstance();
  TvnServerInfo info;
  srv->getServerInfo(&info);
  

  memset(&repeaterHandshake,0,250);
  StringStorage repeater;
  repeater.format(_T("ID:%s"), info.m_repeater.getString());
  AnsiStringStorage ansiRepeater(&repeater);
  memcpy(&repeaterHandshake,ansiRepeater.getString(),ansiRepeater.getLength());
  m_output->writeFully(&repeaterHandshake,250);
  srv->setRepeaterStatus(new StringStorage(_T(":Ready")));

  RfbInitializer::initVersion();
}
