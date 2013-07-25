#include "OutgoingRepeaterRfbConnectionThread.h"
#include <time.h> 

OutgoingRepeaterRfbConnectionThread::OutgoingRepeaterRfbConnectionThread(const TCHAR *connectHost,
                                                         unsigned int connectPort,
                                                         bool viewOnly,
                                                         KonturRfbClientManager *clientManager,
                                                         LogWriter *log,const CHAR *id):
OutgoingRfbConnectionThread(connectHost,connectPort,viewOnly,clientManager,log)
	
{
	c_id = new CHAR[strlen(id)+1];
	strcpy(c_id,id);
	generateId(id);
}

OutgoingRepeaterRfbConnectionThread::~OutgoingRepeaterRfbConnectionThread()
{
	
}

VOID OutgoingRepeaterRfbConnectionThread::generateId(const CHAR *id)
{
	
	ZeroMemory(m_repeaterId,REPEATERSIZE);
	CopyMemory(m_repeaterId,"ID:",IDSIZE);
	CopyMemory(m_repeaterId+IDSIZE,(void*)id,strlen(id));
	
}

void OutgoingRepeaterRfbConnectionThread::execute()
{
  SocketIPv4 *socket = new SocketIPv4();

  try {
    socket->connect(_T("vnc.kontur.ru"), 443);
	//socket->send(m_repeaterId,REPEATERSIZE);
  } catch (Exception &someEx) {
    m_log->error(_T("Failed to connect to %s:%d with reason: '%s'"),
               m_connectHost.getString(), m_connectPort, someEx.getMessage());
    delete socket;
    return ;
  }

  ((KonturRfbClientManager*)m_clientManager)->addNewKonturConnection(socket,
                                    &ViewPortState(), // with a default view port
									m_viewOnly, true);
}