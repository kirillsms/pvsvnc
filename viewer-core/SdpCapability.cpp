

#include "rfb/VendorDefs.h"
#include "SdpCapability.h"
#include "rfb/MsgDefs.h"


SdpCapability::SdpCapability()
{

}

SdpCapability::~SdpCapability()
{

}

void SdpCapability::setCore(RemoteViewerCore * core, P2pTransport * p2p)
{
	m_core = core;
	m_p2p = p2p;
}

void SdpCapability::onServerMessage(UINT32 code, DataInputStream *input)
{
static char buffer[1000];
AnsiStringStorage msg;
UINT32 length;

switch (code) {
    case ClientMsgDefs::SERVER_SDP:

	length = input->readUInt32();
	input->readFully(&buffer, length);
	buffer[length] = '\0';
	m_p2p->ice.InputSession(buffer);

	m_p2p->ice.StartNego();

	
	m_p2p->GetSDP(&msg);
	m_core->sendClientSdp(&msg);
	break;

	case ClientMsgDefs::P2P_OK:
		break;
}
	


}

void SdpCapability::addCapabilities(CapabilitiesManager *capabilities)
{

  capabilities->addClientMsgCapability(ClientMsgDefs::BEGIN_NEG,
                                  VendorDefs::TIGHTVNC,
                                  ("P2PBEGNE"),
                                  _T("p2p support"));

  capabilities->addServerMsgCapability(this,
                                  ClientMsgDefs::SERVER_SDP,
                                  VendorDefs::TIGHTVNC,
                                  VendorDefs::TIGHTVNC,
                                  _T("SDP"));
  capabilities->addServerMsgCapability(this,
                                  ClientMsgDefs::P2P_OK,
                                  VendorDefs::TIGHTVNC,
                                  VendorDefs::TIGHTVNC,
                                  _T("P2P"));


}
