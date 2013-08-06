#pragma once
#include "thread/Thread.h"
#include "avilog/AVIGenerator.h"
#include "log-writer/LogWriter.h"
#include "rfb/FrameBuffer.h"
#include "thread/GlobalMutex.h"

class AvilogThread :
	public Thread
{
public:
	AvilogThread(const FrameBuffer *);
	~AvilogThread();
	void UpdateAvilog();
	GlobalMutex m_mutex;
protected:
	virtual void execute();

	const FrameBuffer * m_frame;
	void *m_buffer;
	BYTE *m_tempbuffer;
	size_t m_bufferLen;
	BITMAPINFOHEADER  bmiHeader;

	CAVIGenerator *m_avilog;
};

