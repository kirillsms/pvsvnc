

#include "rfb/VendorDefs.h"
#include "TextCapability.h"
#include "rfb/MsgDefs.h"


TextCapability::TextCapability()
{

}

TextCapability::~TextCapability()
{

}


void TextCapability::setChatDialog(ClientChatDialog * dialog)
{
	m_clientDialog = dialog;

}
void TextCapability::onServerMessage(UINT32 code, DataInputStream *input)
{

		UINT32 length = input->readUInt32();
		
		std::vector<TCHAR> charBuff(length + 1);

		input->readFully(&charBuff.front(), length);
		charBuff[length] = '\0';
		StringStorage msg(&charBuff.front());

		m_clientDialog->addMsg(msg);

}

void TextCapability::addCapabilities(CapabilitiesManager *capabilities)
{

  capabilities->addServerMsgCapability(this,
                                  ClientMsgDefs::SERVER_CHAT_MSG,
                                  VendorDefs::TIGHTVNC,
                                  VendorDefs::TIGHTVNC,
                                  _T("Text Chat"));


}
