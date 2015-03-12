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
	AvilogThread(const FrameBuffer *, bool isAutoStart);
	~AvilogThread();
	void UpdateAvilog();
	void SetCursorPos(Rect cursorpos);
	void setPort(int port);

	GlobalMutex m_mutex;
	bool m_isRecord;

protected:
	virtual void execute();

	const FrameBuffer * m_frame;
	void *m_buffer;
	BYTE *m_tempbuffer;
	size_t m_bufferLen;
	BITMAPINFOHEADER  bmiHeader;
	Rect crect;	
	int m_port;
	CAVIGenerator *m_avilog;
};
