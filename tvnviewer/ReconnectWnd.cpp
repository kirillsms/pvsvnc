#include "ReconnectWnd.h"
#include "resource.h"
#include "ViewerWindow.h"
#include "TvnViewer.h"

ReconnectDialog::ReconnectDialog(class TvnViewer * pViewer) : 
	BaseDialog(IDD_RECONNECT),
	dwAttempt(1),
	pViewer(pViewer)
{
}

BOOL ReconnectDialog::onInitDialog() {
	setControlById(tbAttempts, IDC_ATTEMPT);
	setControlById(tbInfo, IDC_ADDITIONALINFO);
	return TRUE;
}

VOID ReconnectDialog::incrementAttempt() {
	TCHAR	szAttempts[0x40];
	wsprintf(szAttempts, _T("%d"), dwAttempt);
	dwAttempt++;
	LockWindowUpdate(tbAttempts.getWindow());
	tbAttempts.setText(szAttempts);
	LockWindowUpdate(NULL);
}

VOID ReconnectDialog::destroy() {
	dwAttempt = 1;
	kill(0);
}

VOID ReconnectDialog::setAdditionalInfo(LPTSTR szText) {
	LockWindowUpdate(tbInfo.getWindow());
	tbInfo.setText(szText);
	LockWindowUpdate(NULL);
}

BOOL ReconnectDialog::onCommand(UINT controlID, UINT notificationID) {
	if (controlID == IDCANCEL) {
		pViewer->postMessage(TvnViewer::WM_CONNECTION_CANCELED, TRUE);
		destroy();
		return TRUE;
	}
	return FALSE;
}