#include "OutgoingRepeaterRfbConnectionThread.h"
#include <time.h> 

OutgoingRepeaterRfbConnectionThread::OutgoingRepeaterRfbConnectionThread(const TCHAR *connectHost,
                                                         unsigned int connectPort,
                                                         bool viewOnly,
                                                         RfbClientManager *clientManager,
                                                         LogWriter *log):
OutgoingRfbConnectionThread(connectHost,connectPort,viewOnly,clientManager,log),
	c_id("ID:")
{
	generateId();
}

OutgoingRepeaterRfbConnectionThread::~OutgoingRepeaterRfbConnectionThread()
{
	
}

VOID OutgoingRepeaterRfbConnectionThread::generateId()
{
	srand(time(NULL));
	CHAR* newid = new CHAR[5];
	int id = ((UINT)rand())%10000;
	itoa(id,newid,10);
	ZeroMemory(m_repeaterId,REPEATERSIZE);
	CopyMemory(m_repeaterId,c_id,IDSIZE);
	CopyMemory(m_repeaterId+IDSIZE,(void*)newid,strlen(newid));
	
}

void OutgoingRepeaterRfbConnectionThread::execute()
{
  SocketIPv4 *socket = new SocketIPv4();

  try {
    socket->connect(m_connectHost.getString(), m_connectPort);
	socket->send(m_repeaterId,REPEATERSIZE);
  } catch (Exception &someEx) {
    m_log->error(_T("Failed to connect to %s:%d with reason: '%s'"),
               m_connectHost.getString(), m_connectPort, someEx.getMessage());
    delete socket;
    return ;
  }

  m_clientManager->addNewConnection(socket,
                                    &ViewPortState(), // with a default view port
                                    m_viewOnly, true);
}