#include "FTStatusDialog.h"


#include "tvnserver/resource.h"


FTStatusDialog::FTStatusDialog()
	: BaseDialog(IDD_FT_STATUS)
{
	
}


FTStatusDialog::~FTStatusDialog(void)
{
}



int FTStatusDialog::show()
{
	initControls();
	return BaseDialog::show();
}

void FTStatusDialog::hide()
{
	BaseDialog::hide();
}

void FTStatusDialog::SetStatus(StringStorage status)
{
	SendMessage(hWndLabel, WM_SETTEXT, NULL, (LPARAM) status.getString());
}

void FTStatusDialog::initControls()
{
	HFONT hfFont;
	HWND hwnd = m_ctrlThis.getWindow();

	RECT desktopRect;
    SystemParametersInfo(SPI_GETWORKAREA,0,&desktopRect,0);

	int x = desktopRect.right / 2 + desktopRect.right / 4;
	int y = desktopRect.bottom / 2 - desktopRect.bottom / 4;

	RECT Rect;
	GetWindowRect(hwnd, &Rect);
	SetWindowPos(hwnd, 
					HWND_TOPMOST,
					x,
					y,
					Rect.right - Rect.left,
					Rect.bottom - Rect.top,
					SWP_NOSIZE | SWP_SHOWWINDOW);
	    hfFont = CreateFont(16, 0, 0, 0, 10, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            DEFAULT_PITCH, L"Times New Roman");
		hWndLabel = GetDlgItem(hwnd, IDC_FTSTATUS);
        //hWndLabel = CreateWindowEx( WS_EX_TRANSPARENT, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT | WS_SYSMENU , 10, 10, 145, 20, hwnd, NULL, NULL, NULL);           
        SendMessage(hWndLabel, WM_SETFONT, (WPARAM)hfFont, NULL);  
		
}

BOOL FTStatusDialog::onInitDialog()
{
  
  initControls();
  hide();
  return TRUE;
}


BOOL FTStatusDialog::onCommand(UINT controlID, UINT notificationID)
{
  return FALSE;
}

void FTStatusDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_NCLBUTTONDBLCLK || uMsg == WM_LBUTTONDBLCLK){
		hide();
	}

}


