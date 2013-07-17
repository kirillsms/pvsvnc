#pragma once

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"
#include <list>
#include "RfbClientInfo.h"

class ConnectionStatusDialog : public BaseDialog

{
public:
	ConnectionStatusDialog(void);
	virtual ~ConnectionStatusDialog(void);
	virtual BOOL onInitDialog();
	void refreshClientsList(std::list<RfbClientInfo *> *clients);
	void setTitle(const TCHAR*);

protected:
	void onCloseButtonClick();
	BOOL onCommand(UINT controlID, UINT notificationID);


private:
	void initControls();


protected:
	TextBox m_clientList; 

};

