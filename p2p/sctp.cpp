#include "sctp.h"

bool Sctp::dontbind = false;

void Sctp::StopSctp()
{
	usrsctp_shutdown(sctp->sock,SHUT_RDWR);
	wakeup_one(NULL);
	sctp->terminating = true;
	m_inEvent->notify();
	this->terminate();
}

void Sctp::print_stats()
{
	struct sctpstat stat;
	usrsctp_get_stat(&stat);
	printf("sctps_currestab:%d",stat.sctps_currestab);
	printf("\n");
	printf("sctps_activeestab:%d",stat.sctps_activeestab);
	printf("\n");
	printf("sctps_restartestab:%d",stat.sctps_restartestab);
	printf("\n");
	printf("sctps_collisionestab:%d",stat.sctps_collisionestab);
	printf("\n");
	printf("sctps_passiveestab:%d",stat.sctps_passiveestab);
	printf("\n");
	printf("sctps_aborted:%d",stat.sctps_aborted);
	printf("\n");
	printf("sctps_shutdown:%d",stat.sctps_shutdown);
	printf("\n");
	printf("sctps_outoftheblue:%d",stat.sctps_outoftheblue);
	printf("\n");
	printf("sctps_checksumerrors:%d",stat.sctps_checksumerrors);
	printf("\n");
	printf("sctps_outcontrolchunks:%d",stat.sctps_outcontrolchunks);
	printf("\n");
	printf("sctps_outorderchunks:%d",stat.sctps_outorderchunks);
	printf("\n");
	printf("sctps_outunorderchunks:%d",stat.sctps_outunorderchunks);
	printf("\n");
	printf("sctps_incontrolchunks:%d",stat.sctps_incontrolchunks);
	printf("\n");
	printf("sctps_inorderchunks:%d",stat.sctps_inorderchunks);
	printf("\n");
	printf("sctps_inunorderchunks:%d",stat.sctps_inunorderchunks);
	printf("\n");
	printf("sctps_fragusrmsgs:%d",stat.sctps_fragusrmsgs);
	printf("\n");
	printf("sctps_reasmusrmsgs:%d",stat.sctps_reasmusrmsgs);
	printf("\n");
	printf("sctps_outpackets:%d",stat.sctps_outpackets);
	printf("\n");
	printf("sctps_inpackets:%d",stat.sctps_inpackets);
	printf("\n");
	printf("sctps_recvpackets:%d",stat.sctps_recvpackets);
	printf("\n");
	printf("sctps_recvdatagrams:%d",stat.sctps_recvdatagrams);
	printf("\n");
	printf("sctps_recvpktwithdata:%d",stat.sctps_recvpktwithdata);
	printf("\n");
	printf("sctps_recvsacks:%d",stat.sctps_recvsacks);
	printf("\n");
	printf("sctps_recvdata:%d",stat.sctps_recvdata);
	printf("\n");
	printf("sctps_recvdupdata:%d",stat.sctps_recvdupdata);
	printf("\n");
	printf("sctps_recvheartbeat:%d",stat.sctps_recvheartbeat);
	printf("\n");
	printf("sctps_recvheartbeatack:%d",stat.sctps_recvheartbeatack);
	printf("\n");
	printf("sctps_recvecne:%d",stat.sctps_recvecne);
	printf("\n");
	printf("sctps_recvauth:%d",stat.sctps_recvauth);
	printf("\n");
	printf("sctps_recvauthmissing:%d",stat.sctps_recvauthmissing);
	printf("\n");
	printf("sctps_recvivalhmacid:%d",stat.sctps_recvivalhmacid);
	printf("\n");
	printf("sctps_recvivalkeyid:%d",stat.sctps_recvivalkeyid);
	printf("\n");
	printf("sctps_recvauthfailed:%d",stat.sctps_recvauthfailed);
	printf("\n");
	printf("sctps_recvexpress:%d",stat.sctps_recvexpress);
	printf("\n");
	printf("sctps_recvexpressm:%d",stat.sctps_recvexpressm);
	printf("\n");
	printf("sctps_recvnocrc:%d",stat.sctps_recvnocrc);
	printf("\n");
	printf("sctps_recvswcrc:%d",stat.sctps_recvswcrc);
	printf("\n");
	printf("sctps_recvhwcrc:%d",stat.sctps_recvhwcrc);
	printf("\n");
	printf("sctps_sendpackets:%d",stat.sctps_sendpackets);
	printf("\n");
	printf("sctps_sendsacks:%d",stat.sctps_sendsacks);
	printf("\n");
	printf("sctps_senddata:%d",stat.sctps_senddata);
	printf("\n");
	printf("sctps_sendretransdata:%d",stat.sctps_sendretransdata);
	printf("\n");
	printf("sctps_sendfastretrans:%d",stat.sctps_sendfastretrans);
	printf("\n");
	printf("sctps_sendmultfastretrans:%d",stat.sctps_sendmultfastretrans);
	printf("\n");
	printf("sctps_sendheartbeat:%d",stat.sctps_sendheartbeat);
	printf("\n");
	printf("sctps_sendecne:%d",stat.sctps_sendecne);
	printf("\n");
	printf("sctps_sendauth:%d",stat.sctps_sendauth);
	printf("\n");
	printf("sctps_senderrors:%d",stat.sctps_senderrors);
	printf("\n");
	printf("sctps_sendnocrc:%d",stat.sctps_sendnocrc);
	printf("\n");
	printf("sctps_sendswcrc:%d",stat.sctps_sendswcrc);
	printf("\n");
	printf("sctps_sendhwcrc:%d",stat.sctps_sendhwcrc);
	printf("\n");
	printf("sctps_pdrpfmbox:%d",stat.sctps_pdrpfmbox);
	printf("\n");
	printf("sctps_pdrpfehos:%d",stat.sctps_pdrpfehos);
	printf("\n");
	printf("sctps_pdrpmbda:%d",stat.sctps_pdrpmbda);
	printf("\n");
	printf("sctps_pdrpmbct:%d",stat.sctps_pdrpmbct);
	printf("\n");
	printf("sctps_pdrpbwrpt:%d",stat.sctps_pdrpbwrpt);
	printf("\n");
	printf("sctps_pdrpcrupt:%d",stat.sctps_pdrpcrupt);
	printf("\n");
	printf("sctps_pdrpnedat:%d",stat.sctps_pdrpnedat);
	printf("\n");
	printf("sctps_pdrppdbrk:%d",stat.sctps_pdrppdbrk);
	printf("\n");
	printf("sctps_pdrptsnnf:%d",stat.sctps_pdrptsnnf);
	printf("\n");
	printf("sctps_pdrpdnfnd:%d",stat.sctps_pdrpdnfnd);
	printf("\n");
	printf("sctps_pdrpdiwnp:%d",stat.sctps_pdrpdiwnp);
	printf("\n");
	printf("sctps_pdrpdizrw:%d",stat.sctps_pdrpdizrw);
	printf("\n");
	printf("sctps_pdrpbadd:%d",stat.sctps_pdrpbadd);
	printf("\n");
	printf("sctps_pdrpmark:%d",stat.sctps_pdrpmark);
	printf("\n");
	printf("sctps_timoiterator:%d",stat.sctps_timoiterator);
	printf("\n");
	printf("sctps_timodata:%d",stat.sctps_timodata);
	printf("\n");
	printf("sctps_timowindowprobe:%d",stat.sctps_timowindowprobe);
	printf("\n");
	printf("sctps_timoinit:%d",stat.sctps_timoinit);
	printf("\n");
	printf("sctps_timosack:%d",stat.sctps_timosack);
	printf("\n");
	printf("sctps_timoshutdown:%d",stat.sctps_timoshutdown);
	printf("\n");
	printf("sctps_timoheartbeat:%d",stat.sctps_timoheartbeat);
	printf("\n");
	printf("sctps_timocookie:%d",stat.sctps_timocookie);
	printf("\n");
	printf("sctps_timosecret:%d",stat.sctps_timosecret);
	printf("\n");
	printf("sctps_timopathmtu:%d",stat.sctps_timopathmtu);
	printf("\n");
	printf("sctps_timoshutdownack:%d",stat.sctps_timoshutdownack);
	printf("\n");
	printf("sctps_timoshutdownguard:%d",stat.sctps_timoshutdownguard);
	printf("\n");
	printf("sctps_timostrmrst:%d",stat.sctps_timostrmrst);
	printf("\n");
	printf("sctps_timoearlyfr:%d",stat.sctps_timoearlyfr);
	printf("\n");
	printf("sctps_timoasconf:%d",stat.sctps_timoasconf);
	printf("\n");
	printf("sctps_timodelprim:%d",stat.sctps_timodelprim);
	printf("\n");
	printf("sctps_timoautoclose:%d",stat.sctps_timoautoclose);
	printf("\n");
	printf("sctps_timoassockill:%d",stat.sctps_timoassockill);
	printf("\n");
	printf("sctps_timoinpkill:%d",stat.sctps_timoinpkill);
	printf("\n");
	printf("sctps_hdrops:%d",stat.sctps_hdrops);
	printf("\n");
	printf("sctps_badsum:%d",stat.sctps_badsum);
	printf("\n");
	printf("sctps_noport:%d",stat.sctps_noport);
	printf("\n");
	printf("sctps_badvtag:%d",stat.sctps_badvtag);
	printf("\n");
	printf("sctps_badsid:%d",stat.sctps_badsid);
	printf("\n");
	printf("sctps_nomem:%d",stat.sctps_nomem);
	printf("\n");
	printf("sctps_fastretransinrtt:%d",stat.sctps_fastretransinrtt);
	printf("\n");
	printf("sctps_markedretrans:%d",stat.sctps_markedretrans);
	printf("\n");
	printf("sctps_naglesent:%d",stat.sctps_naglesent);
	printf("\n");
	printf("sctps_naglequeued:%d",stat.sctps_naglequeued);
	printf("\n");
	printf("sctps_maxburstqueued:%d",stat.sctps_maxburstqueued);
	printf("\n");
	printf("sctps_ifnomemqueued:%d",stat.sctps_ifnomemqueued);
	printf("\n");
	printf("sctps_windowprobed:%d",stat.sctps_windowprobed);
	printf("\n");
	printf("sctps_lowlevelerr:%d",stat.sctps_lowlevelerr);
	printf("\n");
	printf("sctps_lowlevelerrusr:%d",stat.sctps_lowlevelerrusr);
	printf("\n");
	printf("sctps_datadropchklmt:%d",stat.sctps_datadropchklmt);
	printf("\n");
	printf("sctps_datadroprwnd:%d",stat.sctps_datadroprwnd);
	printf("\n");
	printf("sctps_ecnereducedcwnd:%d",stat.sctps_ecnereducedcwnd);
	printf("\n");
	printf("sctps_vtagexpress:%d",stat.sctps_vtagexpress);
	printf("\n");
	printf("sctps_vtagbogus:%d",stat.sctps_vtagbogus);
	printf("\n");
	printf("sctps_primary_randry:%d",stat.sctps_primary_randry);
	printf("\n");
	printf("sctps_cmt_randry:%d",stat.sctps_cmt_randry);
	printf("\n");
	printf("sctps_slowpath_sack:%d",stat.sctps_slowpath_sack);
	printf("\n");
	printf("sctps_wu_sacks_sent:%d",stat.sctps_wu_sacks_sent);
	printf("\n");
	printf("sctps_sends_with_flags:%d",stat.sctps_sends_with_flags);
	printf("\n");
	printf("sctps_sends_with_unord:%d",stat.sctps_sends_with_unord);
	printf("\n");
	printf("sctps_sends_with_eof:%d",stat.sctps_sends_with_eof);
	printf("\n");
	printf("sctps_sends_with_abort:%d",stat.sctps_sends_with_abort);
	printf("\n");
	printf("sctps_protocol_drain_calls:%d",stat.sctps_protocol_drain_calls);
	printf("\n");
	printf("sctps_protocol_drains_done:%d",stat.sctps_protocol_drains_done);
	printf("\n");
	printf("sctps_read_peeks:%d",stat.sctps_read_peeks);
	printf("\n");
	printf("sctps_cached_chk:%d",stat.sctps_cached_chk);
	printf("\n");
	printf("sctps_cached_strmoq:%d",stat.sctps_cached_strmoq);
	printf("\n");
	printf("sctps_left_abandon:%d",stat.sctps_left_abandon);
	printf("\n");
	printf("sctps_send_burst_avoid:%d",stat.sctps_send_burst_avoid);
	printf("\n");
	printf("sctps_send_cwnd_avoid:%d",stat.sctps_send_cwnd_avoid);
	printf("\n");
	printf("sctps_fwdtsn_map_over:%d",stat.sctps_fwdtsn_map_over);
	printf("\n");
	printf("sctps_queue_upd_ecne:%d",stat.sctps_queue_upd_ecne);
	printf("\n");

	printf("Number of packets (sent/received): (%u/%u).\n",

	stat.sctps_outpackets, stat.sctps_inpackets);
}

int Sctp::sctp_data_ready_cb(void *reg_addr, void *data, size_t len, uint8_t tos, uint8_t set_df)
{
 struct sctp_transport *sctp = (struct sctp_transport *)reg_addr;
 //printf("sctp_data_ready_cb: %d\n",len);
if(sctp->terminating) return 1;
 sctp_message * msg = create_sctp_message((char *)data,len);
 if(len>1350)
 {
 printf("too big!!");
 }
 AutoLock alt(&sctp->outgoing_mutex);
 {
 sctp->outgoing_queue.push(msg);
 }
 sctp->inEvent->notify();
 

 /*d

 if(BIO_write(sctp->outgoing_bio, data, len)==len){
 sctp->inEvent->notify();
 }else{
 printf("BIO error");
 }*/
  
  return 0;
}

void Sctp::handle_notification_message(union sctp_notification *notify, size_t len,void *user_data)
{
	struct sctp_transport *sctp = (struct sctp_transport *)user_data;

   switch (notify->sn_header.sn_type) {
    case SCTP_ASSOC_CHANGE:
      printf("SCTP_ASSOC_CHANGE\n");
	  
	    switch (notify->sn_assoc_change.sac_state) {
    case SCTP_COMM_UP:
      printf( "Association change SCTP_COMM_UP");
      break;
    case SCTP_COMM_LOST:
      printf( "Association change SCTP_COMM_LOST");
      break;
    case SCTP_RESTART:
      printf(  "Association change SCTP_RESTART");
      break;
    case SCTP_SHUTDOWN_COMP:
      printf(  "Association change SCTP_SHUTDOWN_COMP");
      break;
    case SCTP_CANT_STR_ASSOC:
      printf( "Association change SCTP_CANT_STR_ASSOC");
      break;
    default:
      printf( "Association change UNKNOWN");
      break;
  }


      break;
    case SCTP_REMOTE_ERROR:
	  printf("SCTP_REMOTE_ERROR\n");
      break;
    case SCTP_SHUTDOWN_EVENT:
	  printf("SCTP_SHUTDOWN_EVENT\n");
      break;
    case SCTP_ADAPTATION_INDICATION:
	  printf("SCTP_ADAPTATION_INDICATION\n");
      break;
    case SCTP_PARTIAL_DELIVERY_EVENT:
	  printf("SCTP_PARTIAL_DELIVERY_EVENT\n");
      break;
    case SCTP_AUTHENTICATION_EVENT:
	  printf("SCTP_AUTHENTICATION_EVENT\n");
      break;
    case SCTP_SENDER_DRY_EVENT:
	//  printf("SCTP_SENDER_DRY_EVENT\n");
      break;
    // TODO(ldixon): Unblock after congestion.
    case SCTP_NOTIFICATIONS_STOPPED_EVENT:
	  printf("SCTP_NOTIFICATIONS_STOPPED_EVENT\n");
      break;
    case SCTP_SEND_FAILED_EVENT:
	  printf("SCTP_SEND_FAILED_EVENT\n");
      break;
    case SCTP_STREAM_RESET_EVENT:
      printf("SCTP_STREAM_RESET_EVENT\n");
      break;
    case SCTP_ASSOC_RESET_EVENT:
	  printf("SCTP_ASSOC_RESET_EVENT\n");
      break;
    case SCTP_STREAM_CHANGE_EVENT:
	  printf("SCTP_STREAM_CHANGE_EVENT\n");	
	  break;
    default:
      printf("SCTP_ERROR %d\n",notify->sn_header.sn_type);
	  if(sctp->handshake_done) sctp->error = true;
      break;
  }

}

int Sctp::sctp_data_received_cb(struct socket *sock, union sctp_sockstore addr, void *data, size_t len, struct sctp_rcvinfo recv_info, int flags, void *user_data)
{
	//printf("sctp: %s\n",data);
	//if(len>65000)
		//printf("sctp: %d b\n",len);

	struct sctp_transport *sctp = (struct sctp_transport *)user_data;
	
	if(flags & MSG_NOTIFICATION) {
		handle_notification_message((union sctp_notification *)data, len,user_data);
	}else{

	{
	AutoLock al(&sctp->outToUserEvent_mutex);
	BIO_write(sctp->recv_bio,data,len);
	}
	
	sctp->outToUserEvent.notify();
	free(data); //leak!!

	}
	return 1;
}



Sctp::Sctp(LogWriter *log, Dtls::dtls_transport * dtls_tr, sctp_transport * sctp_, WindowsEvent * outEvent,WindowsEvent * inEvent, int port) {

	//sctp = (struct sctp_transport *)calloc(1, sizeof *sctp);

	m_outEvent = outEvent;
	m_inEvent = inEvent;

	sctp = sctp_;

	sctp->inEvent = m_inEvent;

	if (sctp == NULL) {
		return;
	}
	sctp->error = false;
	sctp->terminating = false;
	sctp->local_port = port;
	sctp->dtls_tr = dtls_tr;

	if(!dontbind) {
		usrsctp_init(0, sctp_data_ready_cb, NULL);
		dontbind = true;
	}

	usrsctp_sysctl_set_sctp_ecn_enable(0);

	usrsctp_register_address(sctp);
	struct socket *s = usrsctp_socket(AF_CONN, SOCK_STREAM, IPPROTO_SCTP,sctp_data_received_cb, NULL, 0, sctp);
	if (s == NULL) {
		goto trans_err;
	}
	sctp->sock = s;

	BIO *bio = BIO_new(BIO_s_mem());
	if (bio == NULL) {
		goto trans_err;
	}
	BIO_set_mem_eof_return(bio, -1);
	sctp->incoming_bio = bio;

	bio = BIO_new(BIO_s_mem());
	if (bio == NULL) {
		goto trans_err;
	}
	BIO_set_mem_eof_return(bio, -1);
	sctp->outgoing_bio = bio;

	bio = BIO_new(BIO_s_mem());
	if (bio == NULL) {
		goto trans_err;
	}
	BIO_set_mem_eof_return(bio, -1);
	sctp->recv_bio = bio;

	struct linger lopt;
	lopt.l_onoff = 1;
	lopt.l_linger = 0;
	usrsctp_setsockopt(s, SOL_SOCKET, SO_LINGER, &lopt, sizeof lopt);

	struct sctp_paddrparams peer_param;
	memset(&peer_param, 0, sizeof peer_param);
	peer_param.spp_assoc_id = 0;
	peer_param.spp_flags = SPP_PMTUD_DISABLE;
	peer_param.spp_pathmtu = 1200;
	usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_PEER_ADDR_PARAMS, &peer_param, sizeof peer_param);

	struct sctp_assoc_value av;
	av.assoc_id = SCTP_ALL_ASSOC;
	av.assoc_value = 1;
	usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_ENABLE_STREAM_RESET, &av, sizeof av);

	uint32_t nodelay = 1;
	usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_NODELAY, &nodelay, sizeof nodelay);

	struct sctp_initmsg init_msg;
	memset(&init_msg, 0, sizeof init_msg);
	init_msg.sinit_num_ostreams = RTCDC_MAX_OUT_STREAM;
	init_msg.sinit_max_instreams = RTCDC_MAX_IN_STREAM;
	usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_INITMSG, &init_msg, sizeof init_msg);

	int event_types[] =  {SCTP_ASSOC_CHANGE,
		SCTP_PEER_ADDR_CHANGE,            
		SCTP_REMOTE_ERROR,                
		SCTP_SEND_FAILED,                 
		SCTP_SHUTDOWN_EVENT,              
		SCTP_ADAPTATION_INDICATION,       
		SCTP_PARTIAL_DELIVERY_EVENT,      
		SCTP_AUTHENTICATION_EVENT,        
		SCTP_STREAM_RESET_EVENT,          
		SCTP_SENDER_DRY_EVENT,            
		SCTP_NOTIFICATIONS_STOPPED_EVENT, 
		SCTP_ASSOC_RESET_EVENT,           
		SCTP_STREAM_CHANGE_EVENT,         
		SCTP_SEND_FAILED_EVENT};           

	struct sctp_event event = {0};
	event.se_assoc_id = SCTP_ALL_ASSOC;
	event.se_on = 1;
	for (size_t i = 0; i < 12; i++) {
		event.se_type = event_types[i];
		usrsctp_setsockopt(s, IPPROTO_SCTP, SCTP_EVENT, &event,sizeof(event));
	}

	struct sockaddr_conn sconn;
	memset(&sconn, 0, sizeof sconn);
	sconn.sconn_family = AF_CONN;
	sconn.sconn_port = htons(sctp->local_port);
	sconn.sconn_addr = (void *)sctp;

	usrsctp_bind(s, (struct sockaddr *)&sconn, sizeof sconn);

	if (0) {
trans_err:
		usrsctp_finish();
		free(sctp);
		sctp = NULL;
	}
	//this->resume();
	return;
}

Sctp::~Sctp(){
	sctp->terminating=true;
	m_inEvent->notify();
	this->terminate();
	this->wait();

if (sctp == NULL)
    return;

  usrsctp_close(sctp->sock);
  usrsctp_deregister_address(sctp);
  BIO_free_all(sctp->incoming_bio);
  BIO_free_all(sctp->outgoing_bio);
  BIO_free_all(sctp->recv_bio);
  sctp = NULL;

}

void Sctp::execute()
{
//	char buf[1200];
	sctp_message * msg;

	while(!isTerminating())
	{

	m_inEvent->waitForEvent(100);
	if(sctp->terminating)
		return;
	
	{
		AutoLock al(&sctp->incoming_mutex);
		while(!sctp->incoming_queue.empty()) 
		{
	
			msg = sctp->incoming_queue.front();

			if(msg==NULL) continue;

			if (msg->len > 0) 
			{
				//printf("sctp->incoming_queue %d\n",msg->len);
				usrsctp_conninput(sctp, msg->data, msg->len, 0);
			}

			destroy_sctp_message(msg);

			msg = NULL;

			sctp->incoming_queue.pop();	
		}
	}

	{
		AutoLock al(&sctp->outgoing_mutex);
		if(!sctp->outgoing_queue.empty()) {
			m_outEvent->notify();
		}
	}

//while(!sctp->outgoing_queue.empty()) 
//{
//		sctp_message * msg;
//		{
//		 AutoLock al(&sctp->mutex);
//		 msg = sctp->outgoing_queue.front();
//		}
//
//		if(msg==NULL) continue;
//
//		printf("sctp->outgoing_queue: %d\n",msg->len);
//
//		if (msg->len > 0) {
//		{
//			AutoLock al(&sctp->dtls_tr->m_mutex);
//			SSL_write(sctp->dtls_tr->ssl, msg->data, msg->len);
//		}
//		m_outEvent->notify();
//        }
//		sctp->outgoing_queue.pop();
//		destroy_sctp_message(msg);
//}

//sleep(10);

}


}


Sctp::sctp_message * Sctp::create_sctp_message(char *buffer, unsigned int length) {
	if(buffer == NULL || length == 0)
		return NULL;
	Sctp::sctp_message *message = (Sctp::sctp_message*)calloc(1, sizeof(Sctp::sctp_message));
	if(message == NULL)
		return NULL;
	message->data = calloc(length, sizeof(char));
	if(message->data == NULL) {
		free(message);
		message = NULL;
		return NULL;
	}
	memcpy(message->data, buffer, length);
	message->len = length;
	return message;
}

void Sctp::destroy_sctp_message(sctp_message *message) {
	if(message == NULL)
		return;
	if(message->data != NULL)
		free(message->data);
	message->data = NULL;
	free(message);
	message = NULL;
}

void Sctp::sctp_incoming_msg(sctp_transport *sctp, void* data, unsigned int len) {
	if(sctp->terminating) return;
	sctp_message * msg = create_sctp_message((char *)data,len);
	{
	AutoLock al(&sctp->incoming_mutex);
	sctp->incoming_queue.push(msg); 
	}

}
