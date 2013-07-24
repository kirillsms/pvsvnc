// Copyright (C) 2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#include "KonturLoginDialog.h"
#include "NamingDefs.h"
#include "OptionsDialog.h"
#include "resource.h"
#include "win-system/Shell.h"

KonturLoginDialog::KonturLoginDialog(TvnViewer *viewer)
: BaseDialog(IDD_KONTURLOGINDIALOG),
  m_viewer(viewer)
{
}

KonturLoginDialog::~KonturLoginDialog()
{
}

BOOL KonturLoginDialog::onInitDialog()
{
  setControlById(m_server, IDC_CREPEATERID);
  setControlById(m_ok, IDOK);
  updateHistory();
  SetForegroundWindow(getControl()->getWindow());
  m_server.setFocus();
  return TRUE;
}

void KonturLoginDialog::enableConnect()
{
  StringStorage str;
  int iSelected = m_server.getSelectedItemIndex();
  if (iSelected == -1) {
    m_server.getText(&str);
    m_ok.setEnabled(!str.isEmpty());
  } else {
    m_ok.setEnabled(true);
  }
}

void KonturLoginDialog::updateHistory()
{
  ConnectionHistory *conHistory;

  StringStorage currentServer;
  m_server.getText(&currentServer);
  m_server.removeAllItems();
  conHistory = ViewerConfig::getInstance()->getConnectionHistory();
  conHistory->load();
  for (size_t i = 0; i < conHistory->getHostCount(); i++) {
    m_server.insertItem(static_cast<int>(i), conHistory->getHost(i));
  }
  m_server.setText(currentServer.getString());
  if (m_server.getItemsCount()) {
    if (currentServer.isEmpty()) {
      m_server.setSelectedItem(0);
    }
    StringStorage server;
    m_server.getText(&server);
    ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                            server.getString());
    m_connectionConfig.loadFromStorage(&ccsm);
  }
}

void KonturLoginDialog::onConnect()
{
  ConnectionHistory *conHistory = ViewerConfig::getInstance()->getConnectionHistory();

  m_server.getText(&m_serverHost);

  conHistory->load();
  conHistory->addHost(m_serverHost.getString());
  conHistory->save();

  if (!m_serverHost.isEmpty()) {
    ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH, m_serverHost.getString());
    m_connectionConfig.saveToStorage(&ccsm);
  }

  m_viewer->newConnection(&m_serverHost, &m_connectionConfig);
}

void KonturLoginDialog::onConfiguration()
{
  m_viewer->postMessage(TvnViewer::WM_USER_CONFIGURATION);
}

BOOL KonturLoginDialog::onOptions()
{
  OptionsDialog dialog;
  dialog.setConnectionConfig(&m_connectionConfig);
  dialog.setParent(getControl());
  if (dialog.showModal() == 1) {
    StringStorage server;
    m_server.getText(&server);
    if (server.isEmpty()) {
      ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                              server.getString());
      m_connectionConfig.saveToStorage(&ccsm);
    }
    return FALSE;
  }
  return TRUE;
}

void KonturLoginDialog::onOrder()
{
  openUrl(StringTable::getString(IDS_URL_LICENSING_FVC));
}

void KonturLoginDialog::openUrl(const TCHAR *url)
{
  // TODO: removed duplicated code (see AboutDialog.h)
  try {
    Shell::open(url, 0, 0);
  } catch (const SystemException &sysEx) {
    StringStorage message;

    message.format(StringTable::getString(IDS_FAILED_TO_OPEN_URL_FORMAT), sysEx.getMessage());

    MessageBox(m_ctrlThis.getWindow(),
               message.getString(),
               StringTable::getString(IDS_MBC_TVNVIEWER),
               MB_OK | MB_ICONEXCLAMATION);
  }
}





void KonturLoginDialog::onAbout()
{
  m_viewer->postMessage(TvnViewer::WM_USER_ABOUT);
}

BOOL KonturLoginDialog::onCommand(UINT controlID, UINT notificationID)
{
  switch (controlID) {
  case IDC_CREPEATERID   :
    switch (notificationID) {
    case CBN_DROPDOWN:
      updateHistory();
      break;

    // select item in ComboBox with list of history
    case CBN_SELENDOK:
      {
        int selectedItemIndex = m_server.getSelectedItemIndex();
        if (selectedItemIndex < 0) {
          return FALSE;
        }
        StringStorage server;
        m_server.getItemText(selectedItemIndex, &server);
        ConnectionConfigSM ccsm(RegistryPaths::VIEWER_PATH,
                                server.getString());
        m_connectionConfig.loadFromStorage(&ccsm);
        break;
      }
    }

    enableConnect();
    break;

  // click "Connect"
  case IDOK:
    onConnect();
    kill(0);
    break;

  // cancel connection
  case IDCANCEL:
    kill(0);
    break;

  case IDC_BCONFIGURATION:
    onConfiguration();
    break;

  case IDC_BOPTIONS:
    return onOptions();

  case IDC_BABOUT:
    onAbout();
    break;

  default:
    _ASSERT(true);
    return FALSE;
  }
  return TRUE;
}

StringStorage KonturLoginDialog::getServerHost()
{
  return m_serverHost;
}
