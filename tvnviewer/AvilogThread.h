#pragma once
#include "thread/Thread.h"
#include "avilog/AVIGenerator.h"
#include "win-system/AnonymousPipeFactory.h"
#include "log-writer/LogWriter.h"
class AvilogThread :
	public Thread
{
public:
	AvilogThread(CAVIGenerator* avigen,LogWriter *log);
	virtual ~AvilogThread(void);
	AnonymousPipe *m_first;
	AnonymousPipe *m_second;
	bool free;
protected:
	CAVIGenerator *m_avigen;
	virtual void execute();
};

