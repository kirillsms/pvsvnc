
#ifndef _TEXT_CAPABILITY_H_
#define _TEXT_CAPABILITY_H_


#include "ServerMessageListener.h"
#include "CapabilitiesManager.h"
#include "tvnviewer/ClientChatDialog.h"

class TextCapability : public ServerMessageListener
{
public:
  TextCapability();
  virtual ~TextCapability();
  void setChatDialog(ClientChatDialog * dialog);
  virtual void onServerMessage(UINT32 code, DataInputStream *input);
  virtual void addCapabilities(CapabilitiesManager *capabilitiesManager);

protected:
	ClientChatDialog * m_clientDialog;

};

#endif
