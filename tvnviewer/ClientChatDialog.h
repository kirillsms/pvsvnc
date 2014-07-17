
#ifndef _CLIENT_CHAT_DIALOG_H_
#define _CLIENT_CHAT_DIALOG_H_

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include "thread/LocalMutex.h"

#include "rfb-sconn/TextMsgListener.h"


class ClientChatDialog :
	public BaseDialog
{
public:
	ClientChatDialog(TextMsgListener * extMsgListener);
	void PositionFlyout();
	virtual ~ClientChatDialog(void);

	virtual BOOL onInitDialog();
        virtual BOOL onCommand(UINT controlID, UINT notificationID);

	void addMsg(StringStorage msg);

	
	int show();
	void hide();
private:
	
	void initControls();
	void Send();
	static LRESULT CALLBACK subEditProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);


	StringStorage chatHist;
	TextBox m_chatlog;
	TextBox m_message;

	TextMsgListener *m_extMsgListener;

	
protected:

	WNDPROC oldEditProc;



};

#endif