// Please do not remove this comment.
//
// URL Encoding/Decodeing class, supports Unicode and ANSI (MFC TCHAR)
//
// Written by Daniel Cohen Gindi, (danielgindi (at) gmail.com)
//
// If you have any comments or questions, feel free to email me.
//
// You may use this class for any purpose, commercial or personal.
//#pragma once

#ifndef __URLENC_H__
#define __URLENC_H__

#include <atlstr.h>


// For ANSI, set default codepage to Hebrew.
// This is used for conversion into UTF-8 from ANSI (Through Unicode)
// You can change this to your codepage.
#define URLENCODE_DEFAULT_ANSI_CODEPAGE 1251

class CURLEncode
{
private:
	static LPCTSTR m_lpszUnsafeString;
	static int m_iUnsafeLen;
	static LPCTSTR m_lpszReservedString;
	static int m_iReservedLen;

	CString toHex(BYTE val);
	WORD toUTF8(TCHAR tc);
	TCHAR fromUTF8(WORD w);

public:
	CString Encode(CString strURL, BOOL bEncodeReserved=FALSE);
	CString Decode(CString strURL);
};

#endif 
