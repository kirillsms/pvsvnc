#include "AvilogThread.h"
#include "client-config-lib/ViewerConfig.h"


AvilogThread::AvilogThread(const FrameBuffer *buff):
	m_avilog(0),m_buffer(0),m_bufferLen(0),m_tempbuffer(0),m_mutex()
{
	m_frame = buff;
	ZeroMemory(&bmiHeader,sizeof(bmiHeader));
}


AvilogThread::~AvilogThread()
{
	this->terminate();
	this->wait();
}

void AvilogThread::execute()
{
	
	while(!isTerminating())
	{
		AutoLock mutex(&m_mutex);
		if(!m_avilog)
		{
			SYSTEMTIME lt;    
			GetLocalTime(&lt);
			TCHAR str[MAX_PATH + 32]; // 29 January 2008 jdp 
			_sntprintf_s(str, sizeof str, _T("%04d-%02d-%02d_%02d-%02d-%02d"), lt.wYear,lt.wMonth,lt.wDay,lt.wHour, lt.wMinute,lt.wSecond);
			_tcscat_s(str,_T("_vnc.avi"));
			
			m_avilog = new CAVIGenerator(str,ViewerConfig::getInstance()->getPathToVLogFile(),ViewerConfig::getInstance()->getPathToLogFile(),&bmiHeader,1);
			


			HRESULT hr = m_avilog->InitEngine();
			if (FAILED(hr))
			{
				m_avilog->ReleaseEngine(); 
				delete m_avilog;
				m_avilog=NULL;
			}
		}
		if(m_avilog)
		{
			CopyMemory(m_tempbuffer, m_frame->getBuffer(), bmiHeader.biSizeImage);
			m_avilog->AddFrame(m_tempbuffer);
		}
		else
		{
			sleep(500);
		}
	}
	m_avilog->ReleaseEngine();
}


void AvilogThread::UpdateAvilog()
{
	    
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biBitCount = m_frame->getBitsPerPixel();
		bmiHeader.biWidth = m_frame->getDimension().width;
		bmiHeader.biHeight = m_frame->getDimension().height;
		bmiHeader.biSizeImage = m_frame->getBufferSize();
		bmiHeader.biPlanes = 1;
		bmiHeader.biCompression = BI_RGB;

		if(m_tempbuffer) 
			delete[] m_tempbuffer;

		m_tempbuffer = new BYTE[bmiHeader.biSizeImage];

		if(m_avilog && m_avilog->GetBitmapHeader()->biSizeImage != bmiHeader.biSizeImage)
		{	
			m_avilog->ReleaseEngine(); 
			delete m_avilog;
			m_avilog=NULL;			
		}
}


