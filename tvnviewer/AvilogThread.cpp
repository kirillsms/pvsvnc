#include "AvilogThread.h"


AvilogThread::AvilogThread(CAVIGenerator* avigen,LogWriter *log):
	m_avigen(avigen),free(true)
{
	AnonymousPipeFactory factory(log);
	factory.generatePipes(&m_first,false,&m_second,false);
}


AvilogThread::~AvilogThread(void)
{
}

void AvilogThread::execute()
{
	if(!isTerminating())
	{
		free=false;
		BYTE * frame;
		m_second->read(&frame,sizeof(frame));
		m_avigen->AddFrame(frame);
		free=true;
	}
}