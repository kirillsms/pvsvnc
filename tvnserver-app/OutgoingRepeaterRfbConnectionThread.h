#pragma once
#include "outgoingrfbconnectionthread.h"
#define REPEATERSIZE 250
#define IDSIZE 3
class OutgoingRepeaterRfbConnectionThread :
	public OutgoingRfbConnectionThread
{
public:
	OutgoingRepeaterRfbConnectionThread(const TCHAR *connectHost, unsigned int connectPort,
                              bool viewOnly, RfbClientManager *clientManager, LogWriter *log);
    virtual ~OutgoingRepeaterRfbConnectionThread();
	const CHAR *c_id;

protected:
	virtual void execute();
	VOID generateId();

private:
	CHAR m_repeaterId[REPEATERSIZE];
	
};

