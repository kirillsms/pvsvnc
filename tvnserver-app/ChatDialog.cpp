#include "ChatDialog.h"

#include "tvnserver/resource.h"
#include "thread/AutoLock.h"
#include "win-system/Environment.h"

#include "util/DateTime.h"


ChatDialog::ChatDialog(TextMsgListener * extMsgListener)
	: BaseDialog(IDD_TEXTCHAT_DLG), m_extMsgListener(extMsgListener)
{
	
}


ChatDialog::~ChatDialog(void)
{
}


void ChatDialog::addMsg(StringStorage msg)
{

	//add time stamp

  StringStorage timeString(_T("[Временно недоступно]"));
  SYSTEMTIME st;
  DateTime dt;
  dt = DateTime::now();
  dt.toUtcSystemTime(&st);

    timeString.format(_T("%.2d:%.2d @ "),
                      st.wHour, st.wMinute);
  
	timeString.appendString(msg.getString());

	chatHist.appendString(timeString.getString());
	m_chatlog.setText(chatHist.getString());
	m_chatlog.setCaretPos(0,m_chatlog.getLineCount());

	m_extMsgListener->onTextMsg(&timeString);

	ShowWindow(m_ctrlThis.getWindow(),SW_RESTORE);

}

int ChatDialog::show()
{
	initControls();
	return BaseDialog::show();;
}

void ChatDialog::hide()
{
	
	BaseDialog::hide();
}

void ChatDialog::initControls()
{
	HWND hwnd = m_ctrlThis.getWindow();

	/*RECT desktopRect;
    SystemParametersInfo(SPI_GETWORKAREA,0,&desktopRect,0);

    RECT rcWindow;
    GetWindowRect(hwnd, &rcWindow);
    
	SIZE sizeWindow = {rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top};

    SetWindowPos(hwnd, HWND_TOPMOST, desktopRect.right-sizeWindow.cx, desktopRect.bottom-sizeWindow.cy, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
	*/

		RECT Rect;
		GetWindowRect(hwnd, &Rect);
		SetWindowPos(hwnd, 
					HWND_TOPMOST,
					Rect.left,
					Rect.top,
					Rect.right - Rect.left,
					Rect.bottom - Rect.top,
					SWP_NOSIZE | SWP_SHOWWINDOW);


	SendMessage(GetDlgItem(m_ctrlThis.getWindow(), IDC_CHATAREA_EDIT),EM_SETREADONLY ,1 ,0);
	setControlById(m_chatlog, IDC_CHATAREA_EDIT);
	setControlById(m_message, IDC_INPUTAREA_EDIT);
	
	

}

LRESULT CALLBACK ChatDialog::subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ChatDialog *_this = reinterpret_cast<ChatDialog *>(GetWindowLongPtr(wnd, GWLP_USERDATA));

	switch (msg) {
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_UNICHAR:
		if(wParam==VK_RETURN){
			if (! (GetKeyState(VK_SHIFT) & 0x8000 ) ){
				_this->Send();
				return 0;
			}
		}
	default:
		return CallWindowProc(_this->oldEditProc, wnd, msg, wParam, lParam);
	}
	//return 0;
}


void ChatDialog::Send()
{
	  StringStorage msg;
	  
	  m_message.getText(&msg);

	  if(msg.getLength()>0){ 
		StringStorage tmp_msg;
		Environment::getComputerName(&tmp_msg);
		m_message.setText(_T(""));
		tmp_msg.appendString(_T(": "));
		tmp_msg.appendString(msg.getString());
		tmp_msg.appendString(_T("\r\n"));
		addMsg(tmp_msg);
	  }

}

BOOL ChatDialog::onInitDialog()
{
  
  initControls();

  	SetWindowLongPtr(GetDlgItem(m_ctrlThis.getWindow(), IDC_INPUTAREA_EDIT), GWLP_USERDATA, (LONG_PTR)this);
	oldEditProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(m_ctrlThis.getWindow(), IDC_INPUTAREA_EDIT), GWLP_WNDPROC, (LONG_PTR)subEditProc);


  
  return TRUE;
}


BOOL ChatDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDC_SEND_B:
	  {
		  Send();
	  }
	  break;
  case IDC_HIDE_B:
      ShowWindow(m_ctrlThis.getWindow(),SW_MINIMIZE);
	  break;
  case IDCANCEL:
      hide();
	  break;
  }
  return FALSE;
}

