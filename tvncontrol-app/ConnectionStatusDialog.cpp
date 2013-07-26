#include "ConnectionStatusDialog.h"
#include "tvnserver/resource.h"

#include "win-system/Shell.h"
#include "win-system/VersionInfo.h"
#include "win-system/Environment.h"

#include "gui/Control.h"

#include "tvnserver/BuildTime.h"
#include "ControlTrayIcon.h"


ConnectionStatusDialog::ConnectionStatusDialog(ControlTrayIcon * tray)
	:BaseDialog(IDD_SHOWCONNECTIONSTATUS), m_tray(tray)
{
}


ConnectionStatusDialog::~ConnectionStatusDialog(void)
{
}


BOOL ConnectionStatusDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDCANCEL:
    onCloseButtonClick();
    break;
  }
  return FALSE;
}

void ConnectionStatusDialog::onCloseButtonClick()
{
	m_tray->onShutdownServerMenuItemClick();
}

BOOL ConnectionStatusDialog::onInitDialog()
{
  initControls();
  return true;
}

void ConnectionStatusDialog::initControls()
{
  HWND hwnd = m_ctrlThis.getWindow();
  m_clientList.setWindow(GetDlgItem(hwnd, IDC_CONNECTIONS));
}

void ConnectionStatusDialog::refreshClientsList(std::list<RfbClientInfo *> *clients)
{
	StringStorage buffer, currentText;

	for (std::list<RfbClientInfo *>::iterator it = clients->begin(); it != clients->end(); it++) {
		buffer.appendString((*it)->m_peerAddr.getString());
		buffer.appendChar(_T('\n'));
    }
	m_clientList.getText(&currentText);
	if(!currentText.isEqualTo(&buffer))	{
		m_clientList.setText(buffer.getString());
	}
}

void ConnectionStatusDialog::setTitle(const TCHAR* title)
{
	SetWindowText(m_ctrlThis.getWindow(),title);
}