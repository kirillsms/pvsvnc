
#include "gui/BaseDialog.h"
#include "gui/ListView.h"
#include "thread/LocalMutex.h"
#include "RfbClientInfo.h"

#ifndef _TRAY_DIALOG_H_
#define _TRAY_DIALOG_H_
class TrayDialog :
	public BaseDialog
{
public:
	TrayDialog(void);
	void PositionFlyout();
	virtual ~TrayDialog(void);
	virtual BOOL onInitDialog();
	virtual void onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	int show();
	void hide();
	void addClients(list<RfbClientInfo *> *clients);
private:
	void initControls();

	volatile bool isVisible;

	ListView m_clientList;

	map<UINT32, int> m_clients;

	bool compare_nocase (RfbClientInfo * first, RfbClientInfo * second) { return first->m_id <= second->m_id;	 }
	bool m_min;
	int m_baseSize;
	mutable LocalMutex m_mapLock;
};

#endif