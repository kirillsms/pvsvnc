// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
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

#include "ViewerConfig.h"

#include "win-system/Environment.h"
#include "win-system/RegistryKey.h"
#include "win-system/Registry.h"

#include "file-lib/File.h"



#ifndef SECURITY_WIN32
#define SECURITY_WIN32
#endif

//#define SECURITY_MAC

#include <security.h>

// FIXME: Duplicate macro, see ConnectionConfig.cpp file
#define TEST_FAIL(C,R) if (!C) { R = false; }

ViewerConfig::ViewerConfig(const TCHAR registryPath[])
: m_logLevel(0), m_listenPort(5500), m_historyLimit(32),
  m_showToolbar(true), m_promptOnFullscreen(true),
  m_conHistory(&m_conHistoryKey, m_historyLimit),
  m_logger(0),m_autoRecord(true),m_askComment(false)
{
  StringStorage registryKey;
  registryKey.format(_T("%s\\History"), registryPath);
  m_conHistoryKey.open(Registry::getCurrentUserKey(),
                       registryKey.getString(),
                       true);


  
  TCHAR name[1024];
  ULONG nameSize = sizeof(name)/sizeof(TCHAR);
  if(GetUserNameEx(EXTENDED_NAME_FORMAT::NameDisplay, name, &nameSize)){
  m_userName.setString(name);
  }else{
  GetUserName(name,&nameSize);
  m_userName.setString(name);
  }
  
  
  StringStorage fullName(m_userName.getString());
  StringStorage nameParts[5];
  size_t namePartsCount;
  if (fullName.split(_T(" "), nameParts, &namePartsCount) && (3 == namePartsCount)) {
	  StringStorage nameSurname;
	  nameSurname.format(_T("%s %s (СКБ Контур)"), nameParts[1].getString(), nameParts[0].getString());
	  m_peerName = nameSurname;
  }else{
	  m_peerName = m_userName;
  }

  
  

}

ViewerConfig::~ViewerConfig()
{
  if (m_logger != 0) {
    try {
      delete m_logger;
    } catch (...) {
    }
  }
}

bool ViewerConfig::loadFromStorage(SettingsManager *storage)
{
  bool loadAllOk = true;

  TEST_FAIL(storage->getInt(_T("LogLevel"), &m_logLevel), loadAllOk);
  setLogLevel(m_logLevel);
  TEST_FAIL(storage->getInt(_T("ListenPort"), &m_listenPort), loadAllOk);
  TEST_FAIL(storage->getInt(_T("HistoryLimit"), &m_historyLimit), loadAllOk);
  setHistoryLimit(m_historyLimit);
  //
  // FIXME: Why registry entry has "NoToolbar" name but
  // meaning of code in what, that is this flag is set then
  // toolbar become visible.
  //

  TEST_FAIL(storage->getBoolean(_T("NoToolbar"), &m_showToolbar), loadAllOk);
  TEST_FAIL(storage->getBoolean(_T("AutoRecord"), &m_autoRecord), loadAllOk);
  TEST_FAIL(storage->getBoolean(_T("AskComment"), &m_askComment), loadAllOk);

  if (storage->getBoolean(_T("SkipFullScreenPrompt"), &m_promptOnFullscreen)) {
    m_promptOnFullscreen = !m_promptOnFullscreen;
  } else {
    loadAllOk = false;
  }

  TEST_FAIL(storage->getString(_T("VideoPath"),&m_pathToVLogFile),loadAllOk);
  StringStorage peerName;
  TEST_FAIL(storage->getString(_T("PeerName"),&peerName),loadAllOk);

  if(peerName.getLength()>0){
	m_peerName = peerName;
  }

  return loadAllOk;
}

bool ViewerConfig::saveToStorage(SettingsManager *storage) const
{
  bool saveAllOk = true;

  TEST_FAIL(storage->setInt(_T("LogLevel"),  m_logLevel), saveAllOk);
  TEST_FAIL(storage->setInt(_T("ListenPort"), m_listenPort), saveAllOk);
  TEST_FAIL(storage->setInt(_T("HistoryLimit"), m_historyLimit), saveAllOk);
  TEST_FAIL(storage->setBoolean(_T("NoToolbar"), m_showToolbar), saveAllOk);
  TEST_FAIL(storage->setBoolean(_T("SkipFullScreenPrompt"), !m_promptOnFullscreen), saveAllOk);
  TEST_FAIL(storage->setBoolean(_T("AutoRecord"), m_autoRecord), saveAllOk);
  TEST_FAIL(storage->setBoolean(_T("AskComment"), m_askComment), saveAllOk);
  
  TEST_FAIL(storage->setString(_T("VideoPath"),m_pathToVLogFile.getString()),saveAllOk);
  TEST_FAIL(storage->setString(_T("PeerName"),m_peerName.getString()),saveAllOk);


  return saveAllOk;
}

void ViewerConfig::setListenPort(UINT16 listenPort)
{
  AutoLock l(&m_cs);
  m_listenPort = listenPort;
}

UINT16 ViewerConfig::getListenPort() const
{
  AutoLock l(&m_cs);
  return m_listenPort;
}

void ViewerConfig::setLogLevel(int logLevel)
{
  AutoLock l(&m_cs);

  if (logLevel < 0) {
    logLevel = 0;
  } else if (logLevel > 9){
    logLevel = 9;
  }

  m_logLevel = logLevel;
  if (m_logger != 0) {
    m_logger->changeLogProps(m_pathToLogFile.getString(), m_logLevel);
  }
}

int ViewerConfig::getLogLevel() const
{
  AutoLock l(&m_cs);
  return m_logLevel;
}

void ViewerConfig::getLogDir(StringStorage *logDir) const
{
  AutoLock l(&m_cs);
  *logDir = m_pathToLogFile;
}


void ViewerConfig::setHistoryLimit(int historyLimit)
{
  AutoLock l(&m_cs);

  if (historyLimit < 0) {
    historyLimit = 0;
  } else if (historyLimit > 1024) {
    historyLimit = 1024;
  }

  m_historyLimit = historyLimit;

  m_conHistory.setLimit(m_historyLimit);
}

int ViewerConfig::getHistoryLimit() const
{
  AutoLock l(&m_cs);
  return m_historyLimit;
}

void ViewerConfig::showToolbar(bool show)
{
  AutoLock l(&m_cs);
  m_showToolbar = show;
}

bool ViewerConfig::isToolbarShown() const
{
  AutoLock l(&m_cs);
  return m_showToolbar;
}




void ViewerConfig::askComment(bool Ask)
{
  AutoLock l(&m_cs);
  m_askComment = Ask;
}

bool ViewerConfig::isAskComment() const
{
  AutoLock l(&m_cs);
  return m_askComment;
}

void ViewerConfig::autoRecord(bool Rec)
{
  AutoLock l(&m_cs);
  m_autoRecord = Rec;
}

bool ViewerConfig::isAutoRecord() const
{
  AutoLock l(&m_cs);
  return m_autoRecord;
}

void ViewerConfig::promptOnFullscreen(bool prompt)
{
  AutoLock l(&m_cs);
  m_promptOnFullscreen = prompt;
}

bool ViewerConfig::isPromptOnFullscreenEnabled() const
{
  AutoLock l(&m_cs);
  return m_promptOnFullscreen;
}

const TCHAR *ViewerConfig::getPathToLogFile() const
{
  AutoLock l(&m_cs);
  return m_pathToLogFile.getString();
}

const TCHAR *ViewerConfig::getFullPathToVLogFile() const
{
  AutoLock l(&m_cs);
  return m_fullpathToVLogFile.getString();
}


const void ViewerConfig::setPathToVLogFile(StringStorage vPath)
{
	m_pathToVLogFile.setString(vPath.getString());
}

const void ViewerConfig::setFullPathToVLogFile(StringStorage vPath)
{
	m_fullpathToVLogFile.setString(vPath.getString());
}


const TCHAR *ViewerConfig::getPathToVLogFile() const
{
  AutoLock l(&m_cs);
  return m_pathToVLogFile.getString();
}

const TCHAR *ViewerConfig::getUserName() const
{
  AutoLock l(&m_cs);
  return m_userName.getString();
}


const void ViewerConfig::setPeerName(StringStorage peer)
{
	m_peerName.setString(peer.getString());
}


const TCHAR *ViewerConfig::getPeerName() const
{
  AutoLock l(&m_cs);
  return m_peerName.getString();
}

ConnectionHistory *ViewerConfig::getConnectionHistory()
{
  AutoLock l(&m_cs);
  return &m_conHistory;
}

Logger *ViewerConfig::initLog(const TCHAR logDir[], const TCHAR logName[])
{
 m_logName = logName;
  StringStorage logFileFolderPath;
  StringStorage appDataPath;
  StringStorage vlogFileFolderPath;
  //StringStorage vlogUserFileFolderPath;
  vlogFileFolderPath = m_pathToVLogFile;

  if(!vlogFileFolderPath.getLength()){
	//vlogFileFolderPath.setString(_T("")); //default
	vlogFileFolderPath.format(_T("\\\\ra-fs\\ra-joint\\TeamViewer Video\\%s"),m_userName.getString());
  }
  
  // After that logFilePath variable will contain path to folder
  // where tvnviewer.log must be located
  if (Environment::getSpecialFolderPath(Environment::APPLICATION_DATA_SPECIAL_FOLDER, &appDataPath)) {
    logFileFolderPath.format(_T("%s\\%s"), appDataPath.getString(), logDir);
  } else {
    logFileFolderPath.format(_T("%s"), logDir);
  }

 // Create TightVNC folder
  {
    File folder(logFileFolderPath.getString());
    folder.mkdir();
  }

  {
	                                    

    if (vlogFileFolderPath.endsWith(_T('\\'))) {
      vlogFileFolderPath.truncate(1);
    }

	File videoFolder(vlogFileFolderPath.getString());

	if(videoFolder.exists()){
	// ok, is dir?
		if(videoFolder.isDirectory()){
			// dir exists.
    	}else{
			// exists file
			vlogFileFolderPath = logFileFolderPath;
		}

	}else{
		if(!videoFolder.mkdir()){
				vlogFileFolderPath = logFileFolderPath;
		}
	}

	


	/*
	if(!(videoFolder.exists() && videoFolder.isDirectory()))
	{
		vlogUserFileFolderPath = logFileFolderPath;
	}
	else{
		
		

		File userVideoFolder(vlogUserFileFolderPath.getString());
		

		if (!userVideoFolder.exists())
			userVideoFolder.mkdir();
	}
	*/
  }

  // Path to log file
  AutoLock l(&m_cs);
  m_pathToLogFile = logFileFolderPath;

  m_pathToVLogFile = vlogFileFolderPath;


  
  if (m_logger != 0) {
    delete m_logger;
  }
  m_logger = new FileLogger(m_pathToLogFile.getString(), logName, m_logLevel, false);
  return m_logger;
}

Logger *ViewerConfig::getLogger()
{
  return m_logger;
}
