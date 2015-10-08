// gpl ))

#include "p2ptransport.h"

#include <Windows.h>
#include <io.h>
#include <fcntl.h> 

int P2pTransport::m_pnum = 0;

void hexdump(void *ptr, int buflen) {
	return;
  unsigned char *buf = (unsigned char*)ptr;
  int i, j;
  for (i=0; i<buflen; i+=16) {
    printf("%06x: ", i);
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%02x ", buf[i+j]);
      else
        printf("   ");
    printf(" ");
    for (j=0; j<16; j++) 
      if (i+j < buflen)
        printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');
    printf("\n");
  }
}


void SetStdOutToNewConsole()
{
    // allocate a console for this app
    AllocConsole();

    // redirect unbuffered STDOUT to the console
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int fileDescriptor = _open_osfhandle((intptr_t)consoleHandle, _O_TEXT);
    FILE *fp = _fdopen( fileDescriptor, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // give the console window a nicer title
    SetConsoleTitle(L"Debug Output");

    // give the console window a bigger buffer size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if ( GetConsoleScreenBufferInfo(consoleHandle, &csbi) )
    {
        COORD bufferSize;
        bufferSize.X = csbi.dwSize.X+150;
        bufferSize.Y = 9999;
        SetConsoleScreenBufferSize(consoleHandle, bufferSize);
    }
}




P2pTransport::P2pTransport(LogWriter *log, bool isControl, P2pEventListener * p2pEventListener):dtls(log,&dtls_tansp),ice(log, &m_outEvent,&m_inEvent, p2pEventListener,++m_pnum),sctp(log,&dtls_tansp,&sctp_tansp,&m_outEvent,&m_inEvent,m_pnum)
{
	m_enable = false;
	m_p2pEventListener = p2pEventListener;
	m_isControl = isControl;
	this->resume();
	//SetStdOutToNewConsole();
	
	ice.Init(m_isControl, &dtls_tansp, &sctp_tansp);
	ice.resume();

}

void P2pTransport::GetSDP(AnsiStringStorage * sdp)
{
	static char buffer[1000];
	int len;
	len = ice.EncodeSession(buffer, sizeof(buffer));
	if (len == -1)
	{
	
		sdp->setString("");

	}
	else
	{

		sdp->setString(buffer);
		
	}
	

}

void P2pTransport::SetSDP(char * sdp)
{
//	len = ice.encode_session(buffer, sizeof(buffer));
//	sdp->setString(buffer);

}


P2pTransport::~P2pTransport()
{
	FreeConsole();
	ice.StopIce();
	sctp.StopSctp();
	this->terminate();
	sctp_tansp.outToUserEvent.notify();
	this->wait();
}

int P2pTransport::send(const char *data, int size)
{

if(size>1200)size=1200;
if (usrsctp_sendv(sctp_tansp.sock, data, size, NULL, 0, NULL, 0, SCTP_SENDV_NOINFO, 0) < 0) {
	
    if (errno == EWOULDBLOCK) {
//		printf("EWOULDBLOCK %d\n",size);
		sleep(1);
	  while (usrsctp_sendv(sctp_tansp.sock, data, size, NULL, 0, NULL, 0, SCTP_SENDV_NOINFO, 0) < 0) {
	//	  printf("EWOULDBLOCK again ))\n");
		  sleep(1);
	  }
	  return size;
      
    } else {
      printf("sctp_send_error %d\n", errno);
    }
	return -1; //printf("send failed\n");
}else{
		return size;
}

}

int P2pTransport::recv(char *buffer, int size)
{
	while(BIO_ctrl_pending(sctp_tansp.recv_bio) <= 0 ){
		sctp_tansp.outToUserEvent.waitForEvent(1000);
		if(isTerminating() || sctp_tansp.error) {return -1;}
	}

	int nbytes;
	{
		AutoLock al(&sctp_tansp.outToUserEvent_mutex);
		nbytes = BIO_read(sctp_tansp.recv_bio, buffer, size);
	}
	
	return nbytes;
}


void P2pTransport::execute()
{
	sctp_tansp.handshake_done = FALSE;

	struct sockaddr_conn sconn;
	memset(&sconn, 0, sizeof sconn);
	sconn.sconn_family = AF_CONN;
	sconn.sconn_port = htons(m_pnum);
	sconn.sconn_addr = (void *)&sctp_tansp;

	if(m_isControl) {

		while(!isTerminating())
		{
			if(dtls_tansp.handshake_done && !sctp_tansp.handshake_done){
				sconn.sconn_port = htons(ice.m_pnum);
				printf("connecting sctp...\n");
				sctp.resume();
				if (usrsctp_connect(sctp_tansp.sock, (struct sockaddr *)&sconn, sizeof sconn) < 0) {
					printf ("sctp handshake failed!");
					if(m_p2pEventListener)
						m_p2pEventListener->onP2pFailed();
					return;
				} else {
					sctp_tansp.handshake_done = TRUE;
					printf ("sctp handshake done!\n");
					if(m_p2pEventListener)
						m_p2pEventListener->onP2pSuccess();
					return;
				}
			}
			sleep(100);
		}
	} else {
		sctp.resume();
		printf("Listening sctp socket...\n");
		usrsctp_listen(sctp_tansp.sock, 1);
		socklen_t len = sizeof sconn;
		struct socket *s = usrsctp_accept(sctp_tansp.sock, (struct sockaddr *)&sconn, &len);
		if (s) {
			sctp_tansp.handshake_done = TRUE;
			printf ("sctp handshake done!\n");
			if(m_p2pEventListener)
				m_p2pEventListener->onP2pSuccess();
			struct socket *t = sctp_tansp.sock;
			sctp_tansp.sock = s;
			usrsctp_close(t);
			return;
		}

		if(m_p2pEventListener)
			m_p2pEventListener->onP2pFailed();
		return;
	}
}