#pragma once
#include "ViewerInstance.h"
#include "KonturConnectionData.h"
class KonturViewerInstance :
	public ViewerInstance
{
public:
  KonturViewerInstance(WindowsApplication *application,
                 KonturConnectionData *condata,
                 const ConnectionConfig *conConf);

  // creates the viewer instance if we have the socket
  KonturViewerInstance(WindowsApplication *application,
                 KonturConnectionData *condata,
                 const ConnectionConfig *conConf,
                 SocketIPv4 *socket);
  virtual ~KonturViewerInstance(void);
  virtual void start();

protected:
  StringStorage m_konturId;
};

