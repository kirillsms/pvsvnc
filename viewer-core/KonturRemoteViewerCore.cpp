#include "KonturRemoteViewerCore.h"
#include "util/AnsiStringStorage.h"


KonturRemoteViewerCore::KonturRemoteViewerCore(Logger *logger)
{}


KonturRemoteViewerCore::KonturRemoteViewerCore(RfbInputGate *input, RfbOutputGate *output,
                   CoreEventsAdapter *adapter,
                   Logger *logger,
                   bool sharedFlag)
{}

KonturRemoteViewerCore::KonturRemoteViewerCore(const TCHAR *host, UINT16 port,
                   CoreEventsAdapter *adapter,
                   Logger *logger,
                   bool sharedFlag)
{}

KonturRemoteViewerCore::KonturRemoteViewerCore(SocketIPv4 *socket,
                   CoreEventsAdapter *adapter,
                   Logger *logger,
                   bool sharedFlag)
{}

KonturRemoteViewerCore::~KonturRemoteViewerCore(void)
{
}


void KonturRemoteViewerCore::setID(const StringStorage *id)
{
  m_id = *id;
}

StringStorage KonturRemoteViewerCore::getProtocolString() const
{
  StringStorage protocolString;
  protocolString.format(_T("RFB %03d.%03d\n"), 3, 8);
  return protocolString;
}


void KonturRemoteViewerCore::handshake()
{
  char serverProtocol[13];
  char repeaterHandshake[250];
  serverProtocol[12] = 0;


  m_input->readFully(serverProtocol, 12);
  memset(&repeaterHandshake,0,250);
  StringStorage repeater;
  repeater.format(_T("ID:%s"), m_id.getString());
  AnsiStringStorage ansiRepeater(&repeater);
  memcpy(&repeaterHandshake,ansiRepeater.getString(),ansiRepeater.getLength());
  m_output->writeFully(&repeaterHandshake,250);

  AnsiStringStorage clientProtocolAnsi;
  clientProtocolAnsi.fromStringStorage(&getProtocolString());
  m_output->writeFully(clientProtocolAnsi.getString(), 12);
  m_output->flush();
}