#include "TrayDialog.h"

#include "tvnserver/resource.h"
#include "thread/AutoLock.h"



TrayDialog::TrayDialog(void)
	: BaseDialog(IDD_TRAYOUT),isVisible(false),m_min(false),m_baseSize(0)
{
}


TrayDialog::~TrayDialog(void)
{
}

void TrayDialog::PositionFlyout()
{
	HWND hwnd = m_ctrlThis.getWindow();

	RECT desktopRect;
    SystemParametersInfo(SPI_GETWORKAREA,0,&desktopRect,0);

    RECT rcWindow;
    GetWindowRect(hwnd, &rcWindow);
    
	SIZE sizeWindow = {rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top};

    SetWindowPos(hwnd, HWND_TOPMOST, desktopRect.right-sizeWindow.cx, desktopRect.bottom-sizeWindow.cy, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

	m_baseSize = rcWindow.bottom - rcWindow.top;
}

int TrayDialog::show()
{
	int returnValue(0);
	if(!isVisible){

		returnValue = BaseDialog::show();
		PositionFlyout();
		isVisible = true;
	}
	return returnValue;
}

void TrayDialog::hide()
{
	if(isVisible){

		BaseDialog::hide();
		isVisible = false;
	}
	
	
}

void TrayDialog::initControls()
{
	HWND hwnd = m_ctrlThis.getWindow();

	m_clientList.setWindow(GetDlgItem(hwnd, IDC_CLIENT_LIST));

	m_clientList.addColumn(0, _T("Èìÿ"), 190);
}

void TrayDialog::addClients(list<RfbClientInfo *> *clients)
{
	AutoLock al(&m_mapLock);
	map<UINT32, int> tmpClients = m_clients;
	bool modified=false;
	for (std::list<RfbClientInfo *>::iterator it = clients->begin(); it != clients->end(); it++) {
		if(!m_clients.count((*it)->m_id))
		{
				modified=true;
				m_clientList.addItem(m_clientList.getCount(),(*it)->m_peerAddr.getString(),(LPARAM)(*it)->m_id);
		}
		else
			tmpClients.erase((*it)->m_id);
    }
	list<int> deleted;

	if(tmpClients.size()){
		modified=true;

	typedef std::map<UINT32, int>::iterator it_type;
		for(it_type iterator = m_clients.begin(); iterator != m_clients.end(); iterator++) {
			deleted.push_back(iterator->second);
		}



		deleted.sort();
		for (std::list<int>::reverse_iterator it = deleted.rbegin(); it != deleted.rend(); it++) {
			m_clientList.removeItem(*it);
		}
	}

	if(modified){
		m_clients.clear();
		for(int i=0; i < m_clientList.getCount(); i++){
			ListViewItem it = m_clientList.getItem(i);
			m_clients.insert(std::pair<UINT32,int>(m_clientList.getItemData(i),i));
		}
	}
	
}

BOOL TrayDialog::onInitDialog()
{
  
  initControls();
  
  return TRUE;
}

void TrayDialog::onMessageReceived(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_NCLBUTTONDBLCLK){
		if(m_min){
			//full size
			HWND hwnd = m_ctrlThis.getWindow();
		    RECT rcWindow;
		    GetWindowRect(hwnd, &rcWindow);
			MoveWindow(hwnd,rcWindow.left,rcWindow.top,(rcWindow.right-rcWindow.left),m_baseSize,true);
			m_min=false;
		}else{
			//minsize
			HWND hwnd = m_ctrlThis.getWindow();
		    RECT rcWindow;
		    GetWindowRect(hwnd, &rcWindow);
			MoveWindow(hwnd,rcWindow.left,rcWindow.top,(rcWindow.right-rcWindow.left),GetSystemMetrics(SM_CYCAPTION),true);
			m_min=true;
		}
	}

}

