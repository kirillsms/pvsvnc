#pragma once

#include "gui/BaseDialog.h"
#include "gui/TextBox.h"

struct ReconnectDialog : public BaseDialog {
	DWORD					dwAttempt;
	class TvnViewer *		pViewer;
	
	TextBox					tbAttempts;
	TextBox					tbInfo;

	CRITICAL_SECTION		csErrorInfo;
	LPTSTR					szErrorInfo;

	ReconnectDialog(class TvnViewer * pViewer);

	VOID			setAdditionalInfo(LPTSTR szText);
	VOID			destroy();
	VOID			incrementAttempt();
	VOID			storeAdditionalInfo(LPTSTR szText);
	virtual BOOL	onInitDialog();
	virtual BOOL	onCommand(UINT controlID, UINT notificationID);
};