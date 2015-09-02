#include "StatSender.h"
#include <time.h>
#include "client-config-lib/ViewerConfig.h"
#include "util/URLEncode.h"
#include "p2p/p2ptransport.h"

const int sendStatInterval = 30 * 1000;

StatSender::StatSender(ConnectionData * condata, P2pTransport * p2p)
{

	TCHAR name[256];
	ULONG nameSize = sizeof(name)/sizeof(TCHAR);

	m_p2p = p2p;

	CURLEncode cEncoder;

	GetUserName(name,&nameSize);
	StringStorage username;
	StringStorage videoPath;
	videoPath.setString(cEncoder.Encode(ViewerConfig::getInstance()->getPathToVLogFile()));
	username.setString(cEncoder.Encode(name));
	ausername.fromStringStorage(&username);
	avp.fromStringStorage(&videoPath);
	
	m_condata = condata;
	
	resume();
}

void StatSender::setComment(StringStorage * comment)
{
	CURLEncode cEncoder;
	StringStorage tstr;
	tstr.setString(cEncoder.Encode(comment->getString()));
	acomment.fromStringStorage(&tstr);
}

StatSender::~StatSender()
{
	terminate();
	wait();
}

void StatSender::onTerminate()
{
	m_sleepTimer.notify();
}

void StatSender::execute()
{
	AnsiStringStorage status;
	unsigned int startTime = time(NULL);
	int as = 0;
	int isp2p = 0;
	if(ViewerConfig::getInstance()->isAutoRecord())
		as = 1;
	while(!isTerminating()){
		m_sleepTimer.waitForEvent(sendStatInterval);
		isp2p = 0;
		if(m_p2p && m_p2p->m_enable == true)
			isp2p = 2;
		else if(m_p2p && m_p2p->sctp_tansp.handshake_done == true)
			isp2p = 1;

		status.format("GET /?host=%d&st=%u&uname=%s&vp=%s&as=%d&c=%s&p2p=%d HTTP/1.1\r\nHost: sfl.ru\r\nConnection: close\r\n\r\n",m_condata->getPort(),startTime,ausername.getString(),avp.getString(),as,acomment.getString(),isp2p);
	    SocketAddressIPv4 ipAddress(_T("sfl.ru"), 80);
	    SocketIPv4 m_socket;
	    m_socket.connect(ipAddress);
		m_socket.send(status.getString(),status.getLength());
		m_socket.close();
	}

}