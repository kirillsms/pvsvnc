// Copyright (C) den


#ifndef _P2PTRANSPORT_H_
#define _P2PTRANSPORT_H_

#include "log-writer/LogWriter.h"
#include "util/AnsiStringStorage.h"
#include "win-system/WindowsEvent.h"

#include "ice.h"
#include "sctp.h"
#include "dtls.h"
#include "P2pEventListener.h"
class P2pTransport:
	public Thread
{
public:
  P2pTransport(LogWriter *log, bool isControl,P2pEventListener * p2pEventListener);
  ~P2pTransport();
  void GetSDP(AnsiStringStorage * sdp);
  void SetSDP(char * sdp);
  int send(const char *data, int size);
  int recv(char *buffer, int size);
  Ice ice;
  Dtls dtls;
  Dtls::dtls_transport dtls_tansp;
  Sctp sctp;
  Sctp::sctp_transport sctp_tansp;
  WindowsEvent m_outEvent;
  WindowsEvent m_inEvent;
  bool m_enable;
  static int m_pnum;
  
private:
	bool m_isControl;
	P2pEventListener * m_p2pEventListener;
	 
protected:
	virtual void execute();


};

#endif // _P2PTRANSPORT_H_
