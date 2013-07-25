#pragma once
#include "outgoingrfbconnectionthread.h"
#include "KonturRfbClientManager.h"
#define REPEATERSIZE 250
#define IDSIZE 3
class OutgoingRepeaterRfbConnectionThread :
	public OutgoingRfbConnectionThread
{
public:
	OutgoingRepeaterRfbConnectionThread(const TCHAR *connectHost, unsigned int connectPort,
                              bool viewOnly, KonturRfbClientManager *clientManager, LogWriter *log, const CHAR *id);
    virtual ~OutgoingRepeaterRfbConnectionThread();
	CHAR *c_id;

protected:
	virtual void execute();
	VOID generateId(const CHAR *id);

private:
	CHAR m_repeaterId[REPEATERSIZE];
	
};

