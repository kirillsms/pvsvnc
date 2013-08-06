#pragma once
#include "RemoteViewerCore.h"
#include "tvnviewer/AvilogThread.h"
class KonturRemoteViewerCore :
	public RemoteViewerCore
{
public:
  KonturRemoteViewerCore(Logger *logger = 0);


  KonturRemoteViewerCore(RfbInputGate *input, RfbOutputGate *output,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  KonturRemoteViewerCore(const TCHAR *host, UINT16 port,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  KonturRemoteViewerCore(SocketIPv4 *socket,
                   CoreEventsAdapter *adapter,
                   Logger *logger = 0,
                   bool sharedFlag = true);

  virtual ~KonturRemoteViewerCore(void);

  void setID(const StringStorage *);

  virtual StringStorage getProtocolString() const;

protected:
  virtual void setFbProperties(const Dimension *fbDimension,
                       const PixelFormat *fbPixelFormat);

  StringStorage m_id;
  AvilogThread m_avilog;

private:
  virtual void handshake();

};

