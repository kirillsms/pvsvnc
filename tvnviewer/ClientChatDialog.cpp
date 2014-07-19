#include "ClientChatDialog.h"

#include "resource.h"
#include "thread/AutoLock.h"

#include <client-config-lib/ViewerConfig.h>

ClientChatDialog::ClientChatDialog(	TextMsgListener * MsgListener)
	: BaseDialog(IDD_CLIENTTEXTCHAT_DLG), m_extMsgListener(MsgListener)
{
}

ClientChatDialog::~ClientChatDialog(void)
{
}

void ClientChatDialog::addMsg(StringStorage msg)
{
	chatHist.appendString(msg.getString());
	m_chatlog.setText(chatHist.getString());
	m_chatlog.setCaretPos(0,m_chatlog.getLineCount());
	ShowWindow(m_ctrlThis.getWindow(),SW_RESTORE);
}


int ClientChatDialog::show()
{
	initControls();
	return BaseDialog::show();;
}

void ClientChatDialog::hide()
{
	BaseDialog::hide();
}

void ClientChatDialog::initControls()
{
	HWND hwnd = m_ctrlThis.getWindow();


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


BOOL ClientChatDialog::onInitDialog()
{

	initControls();

	SetWindowLongPtr(GetDlgItem(m_ctrlThis.getWindow(), IDC_INPUTAREA_EDIT), GWLP_USERDATA, (LONG_PTR)this);
	oldEditProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(m_ctrlThis.getWindow(), IDC_INPUTAREA_EDIT), GWLP_WNDPROC, (LONG_PTR)subEditProc);

	return TRUE;
}

LRESULT CALLBACK ClientChatDialog::subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	ClientChatDialog *_this = reinterpret_cast<ClientChatDialog *>(GetWindowLongPtr(wnd, GWLP_USERDATA));

	switch (msg)
	{

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
}



void ClientChatDialog::Send()
{
	StringStorage msg;

	m_message.getText(&msg);

	if(msg.getLength()>0){ 

		StringStorage fullName(ViewerConfig::getInstance()->getPeerName());
		m_message.setText(_T(""));
		fullName.appendString(_T(": "));
		fullName.appendString(msg.getString());
		fullName.appendString(_T("\r\n"));
		m_extMsgListener->onTextMsg(&fullName);
	}
}

BOOL ClientChatDialog::onCommand(UINT controlID, UINT notificationID)
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

