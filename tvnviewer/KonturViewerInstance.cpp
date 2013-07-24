#include "KonturViewerInstance.h"

KonturViewerInstance::KonturViewerInstance(WindowsApplication *application,
                 KonturConnectionData *condata,
                 const ConnectionConfig *conConf)
: ViewerInstance(application, condata, conConf),
 m_konturId(condata->getID())
{

}

  // creates the viewer instance if we have the socket
KonturViewerInstance::KonturViewerInstance(WindowsApplication *application,
                 KonturConnectionData *condata,
                 const ConnectionConfig *conConf,
				 SocketIPv4 *socket)
: ViewerInstance(application, condata, conConf, socket),
 m_konturId(condata->getID())
{

}

KonturViewerInstance::~KonturViewerInstance(void)
{
}

void KonturViewerInstance::start()
{
  Logger *logger = ViewerConfig::getInstance()->getLogger();
  m_viewerWnd.setRemoteViewerCore(&m_viewerCore);
  
  
  m_viewerWnd.setFileTransfer(&m_fileTransfer);
  
  m_vncAuthHandler.addAuthCapability(&m_viewerCore);
  
  m_fileTransfer.addCapabilities(&m_viewerCore);
  

  if (m_socket) {
	if(!m_konturId.isEmpty()) 
		m_viewerCore.setID(&m_konturId);

    m_viewerCore.start(m_socket,
                       &m_viewerWnd, m_conConf.getSharedFlag());
  } else {
	if(!m_konturId.isEmpty()) 
		m_viewerCore.setID(&m_konturId);

    StringStorage strHost;
    m_condata.getReducedHost(&strHost);
    UINT16 portVal = m_condata.getPort();
    m_viewerCore.start(strHost.getString(), portVal,
                       &m_viewerWnd, m_conConf.getSharedFlag());
  }
}
