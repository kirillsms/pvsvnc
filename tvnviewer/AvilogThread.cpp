#include "AvilogThread.h"
#include "client-config-lib/ViewerConfig.h"


AvilogThread::AvilogThread(const FrameBuffer *buff):
	m_avilog(0),m_buffer(0),m_bufferLen(0),m_tempbuffer(0)
{
	UpdateAvilog(buff);
}


AvilogThread::~AvilogThread()
{

}

void AvilogThread::execute()
{
	
	while(!isTerminating())
	{
		
		if(m_tempbuffer && m_avilog)
		{
			memcpy(m_tempbuffer,m_buffer,m_bufferLen);
			m_avilog->AddFrame((BYTE*)m_tempbuffer);
		}
	}
	m_avilog->ReleaseEngine();
}


void AvilogThread::UpdateAvilog(const FrameBuffer *buffer)
{
		
	    BITMAPINFOHEADER  bmiHeader;
		ZeroMemory(&bmiHeader,sizeof(bmiHeader));
		bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmiHeader.biBitCount = buffer->getBitsPerPixel();
		bmiHeader.biWidth = buffer->getDimension().width;
		bmiHeader.biHeight = buffer->getDimension().height;
		bmiHeader.biSizeImage = buffer->getBufferSize();
		bmiHeader.biPlanes = 1;
		bmiHeader.biCompression = BI_RGB;
		
		if(!m_avilog || 0 != memcmp(&bmiHeader, m_avilog->GetBitmapHeader(),sizeof(BITMAPINFOHEADER))){
		    m_bufferLen = buffer->getBufferSize();
			m_buffer = buffer->getBuffer();
			if(m_tempbuffer){
				delete[] m_tempbuffer;
			}
			m_tempbuffer = new BYTE[m_bufferLen];

			
			SYSTEMTIME lt;    
			GetLocalTime(&lt);
			TCHAR str[MAX_PATH + 32]; // 29 January 2008 jdp 
			_sntprintf(str, sizeof str, _T("%02d_%02d_%02d_%02d_%02d"), lt.wMonth,lt.wDay,lt.wHour, lt.wMinute,lt.wSecond);
			_tcscat(str,_T("_vnc.avi"));
			this->m_avilog = new CAVIGenerator(str,ViewerConfig::getInstance()->getPathToLogFile(),&bmiHeader,1);
			HRESULT hr;
			hr=m_avilog->InitEngine();
			if (FAILED(hr))
			{
				m_avilog->ReleaseEngine(); 
				delete m_avilog;
				m_avilog=NULL;
			}
		}
		
}


