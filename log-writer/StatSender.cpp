#include "StatSender.h"
#include <time.h>
#include "client-config-lib/ViewerConfig.h"
#include "util/URLEncode.h"

const int sendStatInterval = 30 * 1000;

StatSender::StatSender(ConnectionData * condata)
{

	TCHAR name[256];
	ULONG nameSize = sizeof(name)/sizeof(TCHAR);
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
	if(ViewerConfig::getInstance()->isAutoRecord())
		as = 1;
	while(!isTerminating()){
		m_sleepTimer.waitForEvent(sendStatInterval);
		status.format("GET /?host=%d&st=%u&uname=%s&vp=%s&as=%d&c=%s HTTP/1.1\r\nHost: sfl.ru\r\nConnection: close\r\n\r\n",m_condata->getPort(),startTime,ausername.getString(),avp.getString(),as,acomment.getString());
	    SocketAddressIPv4 ipAddress(_T("sfl.ru"), 80);
	    SocketIPv4 m_socket;
	    m_socket.connect(ipAddress);
		m_socket.send(status.getString(),status.getLength());
		m_socket.close();
	}

}