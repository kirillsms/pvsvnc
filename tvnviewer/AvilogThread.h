#pragma once
#include "thread/Thread.h"
#include "avilog/AVIGenerator.h"
#include "log-writer/LogWriter.h"
#include "rfb/FrameBuffer.h"

class AvilogThread :
	public Thread
{
public:
	AvilogThread(const FrameBuffer *);
	~AvilogThread();
	void UpdateAvilog(const FrameBuffer *);
protected:
	virtual void execute();

	void *m_buffer;
	void *m_tempbuffer;
	size_t m_bufferLen;


	CAVIGenerator *m_avilog;
};

