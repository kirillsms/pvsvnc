#include "AVIGenerator.h"
#include "revel.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

CAVIGenerator::CAVIGenerator(LPCTSTR sFileName, LPCTSTR sPath,LPBITMAPINFOHEADER lpbih)
:m_pAVIFile(NULL), m_pStream(NULL), m_pStreamCompressed(NULL), released(false),m_dwRate(10)
{
		_tcscpy_s(m_sFile,sPath);
		_tcscat_s(m_sFile,_T("\\"));
		_tcscat_s(m_sFile,sFileName);
		MakeExtAvi();
		SetBitmapHeader(lpbih);
}

CAVIGenerator::CAVIGenerator(LPCTSTR sFileName, LPBITMAPINFOHEADER lpbih)
:m_pAVIFile(NULL), m_pStream(NULL), m_pStreamCompressed(NULL), released(false),m_dwRate(10)
{
		_tcscpy_s(m_sFile,sFileName);
		MakeExtAvi();
		SetBitmapHeader(lpbih);
}


CAVIGenerator::~CAVIGenerator()
{
//	if (tempbuffer) delete []tempbuffer;
//	tempbuffer=NULL;
}

void CAVIGenerator::SetBitmapHeader(LPBITMAPINFOHEADER lpbih)
{
	memcpy(&m_bih,lpbih, sizeof(BITMAPINFOHEADER));
	
	//tempbuffer = new unsigned char[m_bih.biSizeImage];
}

HRESULT CAVIGenerator::InitEngine()
{
	Revel_Error revError;
	Revel_Params revParams;

	Revel_InitializeParams(&revParams);
    revParams.width = m_bih.biWidth;
    revParams.height =m_bih.biHeight;
    revParams.frameRate = 10.0f;
    revParams.quality = 0.5f;
    revParams.codec = REVEL_CD_XVID;
    revParams.hasAudio = 0;

	
    revError = Revel_CreateEncoder(&encoderHandle);
    if (revError != REVEL_ERR_NONE)
    {
	   return -1;
    }
  int len = wcslen(m_sFile);
  char t[1024];
  
  WideCharToMultiByte(CP_ACP, 0, m_sFile, len,
                      t, len, NULL, NULL);
  t[len]='\0';


	revError = Revel_EncodeStart(encoderHandle, t, &revParams);

	if (revError != REVEL_ERR_NONE)
    {
		return -1;
	}

	return 1;

}

void CAVIGenerator::ReleaseEngine()
{
	Revel_Error revError;
    int totalSize;
    revError = Revel_EncodeEnd(encoderHandle, &totalSize);
    if (revError != REVEL_ERR_NONE)
    {
		return;
	}
    Revel_DestroyEncoder(encoderHandle);
	released = true;
	
}
DWORD oldtime=timeGetTime();
HRESULT CAVIGenerator::AddFrame(BYTE *bmBits)
{

	if(released) return 0;
	DWORD newtime=timeGetTime();
	Revel_Error revError;
	;

	INT delta = (1000/m_dwRate)-(newtime-oldtime);
	if (delta>0) {
		Sleep(delta);
		return 0;

	}
	oldtime=newtime;

	    // Draw and encode each frame.
    Revel_VideoFrame frame;
    frame.width = m_bih.biWidth;
    frame.height = m_bih.biHeight;
    frame.bytesPerPixel = m_bih.biBitCount/8;
	if(frame.bytesPerPixel==1)
    frame.pixelFormat = REVEL_PF_PLANAR;  //REVEL_PF_BGRA;
	else
	frame.pixelFormat = REVEL_PF_BGRA;

	frame.pixels = new int[m_bih.biWidth*m_bih.biHeight];
	
	memcpy(frame.pixels, bmBits, m_bih.biSizeImage);
	
    int frameSize;
    revError = Revel_EncodeFrame(encoderHandle, &frame, &frameSize);
    if (revError != REVEL_ERR_NONE)
        {
	        printf("Revel Error while writing frame: %d\n", revError);
	      //  exit(1);
	    }
    
    delete [] (int*)frame.pixels;


	return revError != REVEL_ERR_NONE;
}

void CAVIGenerator::MakeExtAvi()
{
	if( _tcsstr(m_sFile,_T("avi"))==NULL )
	{
		_tcscat_s(m_sFile,_T(".avi"));
	}
}
