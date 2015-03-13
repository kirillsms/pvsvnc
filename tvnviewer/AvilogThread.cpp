#include "AvilogThread.h"
#include "client-config-lib/ViewerConfig.h"


AvilogThread::AvilogThread(const FrameBuffer *buff, bool isAutoStart):
	m_avilog(0),m_buffer(0),m_bufferLen(0),m_tempbuffer(0),m_mutex(),crect(0,0,0,0),m_isRecord(isAutoStart),m_port(0)
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
		
		if(!m_isRecord || m_frame->getBitsPerPixel()<16){
			sleep(500);
			continue;
		}


		AutoLock mutex(&m_mutex);
		if(!m_avilog)
		{
			SYSTEMTIME lt;
			GetLocalTime(&lt);
			TCHAR str[MAX_PATH + 32]; // 29 January 2008 jdp 
			if(_tcslen(ViewerConfig::getInstance()->getFullPathToVLogFile())>0){
			_tcscpy_s(str,ViewerConfig::getInstance()->getFullPathToVLogFile());
			m_avilog = new CAVIGenerator(str,&bmiHeader);
			}else{
			_sntprintf_s(str, sizeof str, _T("%d.%04d-%02d-%02d_%02d-%02d-%02d"), m_port, lt.wYear,lt.wMonth,lt.wDay,lt.wHour, lt.wMinute,lt.wSecond);
			_tcscat_s(str,_T("_vnc.avi"));
			m_avilog = new CAVIGenerator(str,ViewerConfig::getInstance()->getPathToVLogFile(),&bmiHeader);
			}

			

			m_avilog->SetRate(10);

			HRESULT hr = m_avilog->InitEngine();
			//HRESULT hr = 0;
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
			// draw cursor
			Rect rc;			
			rc.top = crect.top;
			rc.bottom = crect.top+5;
			rc.left = crect.left;
			rc.right = crect.left+5;

			// calc inmem pos
			int pixelSize = m_frame->getBytesPerPixel();
			size_t sizeLineFb = m_frame->getBytesPerRow();
			size_t sizeLineRect = rc.getWidth() * pixelSize;
			char *ptr = (char *)m_tempbuffer;
			ptr += (rc.top * m_frame->getDimension().width + rc.left) * pixelSize;
			UINT8 *srcLinePtr = reinterpret_cast<UINT8 *> (ptr);
			UINT8 *pixPtr = srcLinePtr;


if (pixPtr != NULL){

int colour_white = 2147483647;
int colour_black = 0;
for (int x = rc.left; x < rc.right; x++, pixPtr += pixelSize)
	memcpy(pixPtr, &colour_white, pixelSize);
// it's pointer to next line of rect
UINT8 *dstLinePtr = srcLinePtr + sizeLineFb;
for (int y = rc.top + 1; y < rc.bottom; y++, dstLinePtr += sizeLineFb)
    memcpy(dstLinePtr, srcLinePtr, sizeLineRect);

rc.left++;
rc.top++;
rc.right--;
rc.bottom--;

sizeLineRect = rc.getWidth() * pixelSize;

srcLinePtr+=pixelSize+sizeLineFb;
pixPtr = srcLinePtr;
for (int x = rc.left; x < rc.right; x++, pixPtr += pixelSize)
	memcpy(pixPtr, &colour_black, pixelSize);
dstLinePtr = srcLinePtr + sizeLineFb; 
for (int y = rc.top + 1; y < rc.bottom; y++, dstLinePtr += sizeLineFb)
    memcpy(dstLinePtr, srcLinePtr, sizeLineRect);
}

//////// end draw cursor


			m_avilog->AddFrame(m_tempbuffer);
		}
		else
		{
			sleep(500);
		}
	}
	if(m_avilog)
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


void AvilogThread::SetCursorPos(Rect cursorpos)
{
if(m_avilog){
	if(cursorpos.left > 0 && cursorpos.left<m_frame->getDimension().width-5 && cursorpos.top>0 && cursorpos.top<m_frame->getDimension().height-5)
	{
		crect.setRect(&cursorpos);
	}
}
}

void AvilogThread::setPort(int port)
{
m_port = port;

}
