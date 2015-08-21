// Copyright (C) den


#ifndef _SCTP_H_
#define _SCTP_H_

#include <queue>    

#define _WINSOCKAPI_ 

#include "log-writer/LogWriter.h"
#include "thread/Thread.h"
#include "thread/GlobalMutex.h"
#include "thread/Autolock.h"
#include "win-system/WindowsEvent.h"

#define _CRT_SECURE_NO_WARNINGS

#include <usrsctp.h>
#include "dtls.h"

#ifndef RTCDC_MAX_CHANNEL_NUM
#define RTCDC_MAX_CHANNEL_NUM 128
#endif

#ifndef RTCDC_MAX_IN_STREAM
#define RTCDC_MAX_IN_STREAM 1024
#endif

#ifndef RTCDC_MAX_OUT_STREAM
#define RTCDC_MAX_OUT_STREAM 256
#endif

	
class Sctp:
	public Thread
{
public:
	
struct sctp_message {
  void *data;
  unsigned int len;
};

struct sctp_transport {
  struct socket *sock;
  BIO *incoming_bio;
  BIO *outgoing_bio;
  queue<sctp_message *> incoming_queue;
  LocalMutex incoming_mutex;
  queue<sctp_message *> outgoing_queue;
  LocalMutex outgoing_mutex;
  BIO *recv_bio;
  LocalMutex outToUserEvent_mutex;
  int local_port;
  int remote_port;
  bool handshake_done;
  int stream_cursor;
  void *user_data;
  Dtls::dtls_transport * dtls_tr;
  WindowsEvent * inEvent;
  WindowsEvent outToUserEvent;
  bool terminating;
  bool error;
};


	Sctp(LogWriter *log, Dtls::dtls_transport * dtls_tr, Sctp::sctp_transport * sctp, WindowsEvent * outEvent,WindowsEvent * inEvent, int port);
	~Sctp();
	static sctp_message *create_sctp_message(char *buffer, unsigned int length);
	static void destroy_sctp_message(sctp_message *message);
	static void sctp_incoming_msg(sctp_transport *sctp, void* data, unsigned int len);
	void StopSctp();
	void print_stats();
private:
	static int sctp_data_received_cb(struct socket *sock, union sctp_sockstore addr, void *data, size_t len, struct sctp_rcvinfo recv_info, int flags, void *user_data);
	static void handle_notification_message(union sctp_notification *notify, size_t len,void *user_data);
	static int sctp_data_ready_cb(void *reg_addr, void *data, size_t len, uint8_t tos, uint8_t set_df);

	struct sctp_transport *sctp;
	WindowsEvent * m_outEvent;
	WindowsEvent * m_inEvent;
	static bool dontbind;


protected:
	virtual void execute();


};

#endif // _SCTP_H_
