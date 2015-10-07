// gpl ))

#include "ice.h"

const char*   Ice::kStunServer = "stun.l.google.com";
Ice::IceContext Ice::g_ice_stor;
Ice::IceContext * Ice::g_ice = 0;


unsigned char checksum (unsigned char *ptr, size_t sz) {
    unsigned char chk = 0;
    while (sz-- != 0)
        chk -= *ptr++;
    return chk;
}

void hexdump1(void *ptr, int buflen) {
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


#define PRINT(fmt, arg0, arg1, arg2, arg3, arg4, arg5)	    \
	printed = pj_ansi_snprintf(p, maxlen - (p-buffer),  \
				   fmt, arg0, arg1, arg2, arg3, arg4, arg5); \
	if (printed <= 0) return -PJ_ETOOSMALL; \
	p += printed


pj_status_t Ice::HandleEvents(ice_t obj, unsigned max_msec, unsigned *p_count) {


    enum {
        MAX_NET_EVENTS = 1
    };
    pj_time_val max_timeout = {0, 0};
    pj_time_val timeout = { 0, 0};
    unsigned count = 0, net_event_count = 0;
    int c;

    max_timeout.msec = max_msec;

    /* Poll the timer to run it and also to retrieve the earliest entry. */
    timeout.sec = timeout.msec = 0;
    c = pj_timer_heap_poll( obj->ice_cfg.stun_cfg.timer_heap, &timeout );
    if (c > 0) {
        count += c;
    }

    /* timer_heap_poll should never ever returns negative value, or otherwise
     * ioqueue_poll() will block forever!
     */
    pj_assert(timeout.sec >= 0 && timeout.msec >= 0);
    if (timeout.msec >= 1000) {
        timeout.msec = 999;
    }

    /* compare the value with the timeout to wait from timer, and use the
     * minimum value.
     */
    if (PJ_TIME_VAL_GT(timeout, max_timeout)) {
        timeout = max_timeout;
    }

    /* Poll ioqueue.
     * Repeat polling the ioqueue while we have immediate events, because
     * timer heap may process more than one events, so if we only process
     * one network events at a time (such as when IOCP backend is used),
     * the ioqueue may have trouble keeping up with the request rate.
     *
     * For example, for each send() request, one network event will be
     *   reported by ioqueue for the send() completion. If we don't poll
     *   the ioqueue often enough, the send() completion will not be
     *   reported in timely manner.
     */
    do {
        c = pj_ioqueue_poll( obj->ice_cfg.stun_cfg.ioqueue, &timeout);
        if (c < 0) {
            pj_status_t err = pj_get_netos_error();
            pj_thread_sleep((unsigned int)PJ_TIME_VAL_MSEC(timeout));
            if (p_count)
                *p_count = count;
            return err;
        } else if (c == 0) {
            break;
        } else {
            net_event_count += c;
            timeout.sec = timeout.msec = 0;
        }
    } while (c > 0 && net_event_count < MAX_NET_EVENTS);

    count += net_event_count;
    if (p_count) {
        *p_count = count;
    }

    return PJ_SUCCESS;

}


int Ice::WorkerThread(void *arg) {


	ice_t obj = (ice_t) arg;

    while (!obj->thread_quit_flag) {
        HandleEvents(obj, 50, NULL);
    }

    return 0;
}


bool Ice::Init(bool isControl,Dtls::dtls_transport * dtls_transport,Sctp::sctp_transport * sctp_transport ) {

    pj_thread_desc desc;
    pj_thread_t *this_thread;

	pj_thread_register("thread", desc, &this_thread);


pj_ice_sess_role m_role = isControl == false
		? PJ_ICE_SESS_ROLE_CONTROLLING
		: PJ_ICE_SESS_ROLE_CONTROLLED;

		int ret = -1;

		rem.cand_cnt = 0;
	

	do {
		obj = (ice_t) malloc(sizeof(struct IceSt));
        memset(obj, 0, sizeof(struct IceSt));
		
		obj->p2pEventListener = m_p2pEventListener;

		obj->terminating = false;

		obj->dtls_tr = dtls_transport;

		obj->sctp_tr = sctp_transport;

		obj->role = m_role;

		obj->rem_inf = &rem;

		// create pool factory
		pj_caching_pool_init(&obj->cp, NULL, 0);
		obj->cp_inited = 1;

		/* Init our ICE settings with null values */
		pj_ice_strans_cfg_default(&obj->ice_cfg);
		for(int i = 0;i<4;i++){
		obj->ice_cfg.comp[i].so_rcvbuf_size = 24 * 1024 * 1024;
		obj->ice_cfg.comp[i].so_sndbuf_size = 24 * 1024 * 1024;
		}

		// set caching pool factory
		obj->ice_cfg.stun_cfg.pf = &obj->cp.factory;

		/* Create application memory pool */
		obj->ice_pool = pj_pool_create(&obj->cp.factory, "ice_pool",
				512, 512, NULL);
        
		ret = pj_lock_create_recursive_mutex(obj->ice_pool, NULL, &obj->lock);
		if (ret != PJ_SUCCESS) {
			break;
		}


		/* Create timer heap for timer stuff */
		ret = pj_timer_heap_create(obj->ice_pool, 100,
				&obj->ice_cfg.stun_cfg.timer_heap);
		if (ret != PJ_SUCCESS) {
			break;
		}

		/* and create ioqueue for network I/O stuff */
		ret = pj_ioqueue_create(obj->ice_pool, 16, &obj->ice_cfg.stun_cfg.ioqueue);
		if (ret != PJ_SUCCESS) {
			break;
		}

			
		/* something must poll the timer heap and ioqueue,
		 * unless we're on Symbian where the timer heap and ioqueue run
		 * on themselves.
		 */
		ret = pj_thread_create(obj->ice_pool, NULL,
				&WorkerThread, obj, 0, 0, &obj->ice_thread);
		if (ret != PJ_SUCCESS) {
			break;
		}

		obj->ice_cfg.af = pj_AF_INET();


			/* Maximum number of host candidates */
		obj->ice_cfg.stun.max_host_cands = 1;
			/* Nomination strategy */
		obj->ice_cfg.opt.aggressive = PJ_TRUE;

		obj->ice_cfg.stun.server.ptr = strdup(kStunServer);
		obj->ice_cfg.stun.server.slen = strlen(kStunServer);
		obj->ice_cfg.stun.port = 19302;
			/* stun keep alive interval */
		obj->ice_cfg.stun.cfg.ka_interval = 300;
		
		// ice callback
		pj_ice_strans_cb icecb;
		pj_bzero(&icecb, sizeof(icecb));
		icecb.on_ice_complete = OnIceComplete;
		icecb.on_rx_data = OnReceiveData;

            ret = pj_ice_strans_create(NULL, &obj->ice_cfg,
                                1, obj, &icecb,
                                &obj->icest);
		if (ret != PJ_SUCCESS) {
            pj_str_t es = pj_strerror(ret, obj->err_str, sizeof(obj->err_str));

			break;
		}

	}while(0);

	return true;
}

void Ice::OnReceiveData(pj_ice_strans* ice_st, unsigned comp_id, void* pkt, pj_size_t size, const pj_sockaddr_t* src_addr, unsigned src_addr_len) {
	char ipstr[PJ_INET6_ADDRSTRLEN+10];


	ice_t obj = (ice_t) pj_ice_strans_get_user_data(ice_st);

	IO(obj);
	{
		AutoLock al(&obj->dtls_tr->incoming_mutex);
		int written = BIO_write(obj->dtls_tr->incoming_bio, pkt, size);
	}

	IO(obj);

	char data[1500];	/* FIXME */
	memset(&data, 0, 1500);
	int read;
	{
		AutoLock al(&obj->dtls_tr->incoming_mutex);
		read = SSL_read(obj->dtls_tr->ssl, &data, 1500);
	}

	if(read < 0) {
		unsigned long err = SSL_get_error(obj->dtls_tr->ssl, read);
		if(err == SSL_ERROR_SSL) {
			char error[200];
			ERR_error_string_n(ERR_get_error(), error, 200);
			printf("Handshake error: %s\n", error);
			return;
		}else{
		//	printf("ssl error %d %d\n",err,size);
	
		}
	}
	IO(obj);
	if(!SSL_is_init_finished(obj->dtls_tr->ssl)) {
		printf("Initialization not finished yet...\n");
		return;
	}

if(obj->dtls_tr->handshake_done) {
		if(read > 0) {
			if(!obj->terminating){
			Sctp::sctp_incoming_msg(obj->sctp_tr,data,read);
			obj->sctp_tr->inEvent->notify();
			}
		}

} else {
		printf("DTLS established!\n");
		/* Check the remote fingerprint */
		X509 *rcert = SSL_get_peer_certificate(obj->dtls_tr->ssl);
		if(!rcert) {
			printf("No remote certificate?\n");
		} else {
			unsigned int rsize;
			unsigned char rfingerprint[EVP_MAX_MD_SIZE];
			char remote_fingerprint[160];
			char *rfp = (char *)&remote_fingerprint;
			X509_digest(rcert, EVP_sha1(), (unsigned char *)rfingerprint, &rsize);
			X509_free(rcert);
			rcert = NULL;
			unsigned int i = 0;
			for(i = 0; i < rsize; i++) {
				sprintf(rfp, "%.2X:", rfingerprint[i]);
				rfp += 3;
			}
			*(rfp-1) = 0;
			printf("Remote fingerprint of the client is %s\n", remote_fingerprint);
			obj->dtls_tr->handshake_done = 1;
		}
}
}

void Ice::IO(ice_t ice_obj)
{
	if(!g_ice) return;

	RegisterThread(g_ice->pool);

	char outgoing[1500];
	int out;
	int pending = BIO_ctrl_pending(ice_obj->dtls_tr->outgoing_bio);
	while(pending > 0)
	{
		{
			AutoLock al(&ice_obj->dtls_tr->outgoing_mutex);
			out = BIO_read(ice_obj->dtls_tr->outgoing_bio, outgoing, sizeof(outgoing));
		}

		//printf("sending...%d\n",out);
		if(out>0)
		{
			pj_status_t status =  pj_ice_strans_sendto(ice_obj->icest, 1, outgoing, out, &ice_obj->rem_inf->def_addr[0],pj_sockaddr_get_len(&ice_obj->rem_inf->def_addr[0]));
			if(status != PJ_SUCCESS)
			{
				/*printf("error sending, %d...\n",status);
				char errmsg[PJ_ERR_MSG_SIZE];
				pj_strerror(status,errmsg,sizeof(errmsg));
				printf("%s\n",errmsg);*/
				//	status =  pj_ice_strans_sendto(ice_obj->icest, 1, outgoing, out, &ice_obj->rem_inf->def_addr[0],pj_sockaddr_get_len(&ice_obj->rem_inf->def_addr[0]));
			}

			if(status == PJ_EBUSY)
			{
				//sleep(1);
				printf("busy, retring...%d\n",out);
				//status =  pj_ice_strans_sendto(ice_obj->icest, 1, outgoing, out, &ice_obj->rem_inf->def_addr[0],pj_sockaddr_get_len(&ice_obj->rem_inf->def_addr[0]));
			}
		}
		pending = BIO_ctrl_pending(ice_obj->dtls_tr->outgoing_bio);
	}
}


void Ice::OnIceComplete(pj_ice_strans *ice_st, pj_ice_strans_op op,
		pj_status_t status) {

	switch (op) {
	case PJ_ICE_STRANS_OP_INIT: {
		ice_t obj = (ice_t) pj_ice_strans_get_user_data(ice_st);
		pj_lock_acquire(obj->lock);
		pj_status_t ret = pj_ice_strans_init_ice(ice_st, obj->role, NULL, NULL);
		if (status == PJ_SUCCESS){
			obj->ice_init_done = 1;
		}
		else
		{
			obj->ice_init_done = -1;
		}
		obj->ice_init_result = status;
		pj_lock_release(obj->lock);
	}
		break;

	case PJ_ICE_STRANS_OP_NEGOTIATION: {
		ice_t obj = (ice_t) pj_ice_strans_get_user_data(ice_st);
		pj_lock_acquire(obj->lock);
		if(status == PJ_SUCCESS){
		obj->ice_nego_done = 1;
		obj->ice_nego_status = status;
		pj_lock_release(obj->lock);
		// ssl handshake 
		printf("handshake begin\n");
		  if (obj->role == PJ_ICE_SESS_ROLE_CONTROLLING)
				SSL_set_connect_state(obj->dtls_tr->ssl);
			else
				SSL_set_accept_state(obj->dtls_tr->ssl);
				
		  	SSL_do_handshake(obj->dtls_tr->ssl);
			IO(obj);
		}else{
			if(obj->p2pEventListener)
				obj->p2pEventListener->onP2pFailed();
		}
	}
		break;
	default: {

			 }
		break;
	} // end of switch
}

/* Utility to create a=candidate SDP attribute */
int Ice::print_cand(char buffer[], unsigned maxlen,
	const pj_ice_sess_cand *cand)
{
	char ipaddr[PJ_INET6_ADDRSTRLEN];
	char *p = buffer;
	int printed;

	PRINT("a=candidate:%.*s %u UDP %u %s %u typ ",
		(int)cand->foundation.slen,
		cand->foundation.ptr,
		(unsigned)cand->comp_id,
		cand->prio,
		pj_sockaddr_print(&cand->addr, ipaddr,
		sizeof(ipaddr), 0),
		(unsigned)pj_sockaddr_get_port(&cand->addr));

	PRINT("%s\n",
		pj_ice_get_cand_type_name(cand->type),
		0, 0, 0, 0, 0);

	if (p == buffer + maxlen)
		return -PJ_ETOOSMALL;

	*p = '\0';

	return (int)(p - buffer);
}

/*
* Encode ICE information in SDP.
*/
int Ice::EncodeSession(char buffer[], unsigned maxlen)
{
	char *p = buffer;
	unsigned comp;
	int printed;
	pj_str_t local_ufrag, local_pwd;
	pj_status_t status;

	while (obj->ice_init_done<1)
	{
		if (obj->ice_init_done == -1){
			return -1;
		}
		sleep(100);
	}

	/* Write "dummy" SDP v=, o=, s=, and t= lines */
	PRINT("v=0\no=- 3414953978 3414953978 IN IP4 localhost\ns=ice\nt=0 0\n",
		0, 0, 0, 0, 0, 0);

	/* Get ufrag and pwd from current session */
	pj_ice_strans_get_ufrag_pwd(obj->icest, &local_ufrag, &local_pwd,
		NULL, NULL);

	/* Write the a=ice-ufrag and a=ice-pwd attributes */
	PRINT("a=ice-ufrag:%.*s\na=ice-pwd:%.*s\n",
		(int)local_ufrag.slen,
		local_ufrag.ptr,
		(int)local_pwd.slen,
		local_pwd.ptr,
		0, 0);
	
	PRINT("p=%d\n",
		(int)m_pnum,0,0,0,0,0);


	/* Write each component */
	for (comp = 0; comp<1; ++comp) {
		unsigned j, cand_cnt;
		pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];
		char ipaddr[PJ_INET6_ADDRSTRLEN];

		/* Get default candidate for the component */
		status = pj_ice_strans_get_def_cand(obj->icest, comp + 1, &cand[0]);
		if (status != PJ_SUCCESS)
			return -status;

		/* Write the default address */
		if (comp == 0) {
			/* For component 1, default address is in m= and c= lines */
			PRINT("m=audio %d RTP/AVP 0\n"
				"c=IN IP4 %s\n",
				(int)pj_sockaddr_get_port(&cand[0].addr),
				pj_sockaddr_print(&cand[0].addr, ipaddr,
				sizeof(ipaddr), 0),
				0, 0, 0, 0);
		}
		else if (comp == 1) {
			/* For component 2, default address is in a=rtcp line */
			PRINT("a=rtcp:%d IN IP4 %s\n",
				(int)pj_sockaddr_get_port(&cand[0].addr),
				pj_sockaddr_print(&cand[0].addr, ipaddr,
				sizeof(ipaddr), 0),
				0, 0, 0, 0);
		}
		else {
			/* For other components, we'll just invent this.. */
			PRINT("a=Xice-defcand:%d IN IP4 %s\n",
				(int)pj_sockaddr_get_port(&cand[0].addr),
				pj_sockaddr_print(&cand[0].addr, ipaddr,
				sizeof(ipaddr), 0),
				0, 0, 0, 0);
		}

		/* Enumerate all candidates for this component */
		cand_cnt = PJ_ARRAY_SIZE(cand);
		status = pj_ice_strans_enum_cands(obj->icest, comp + 1,
			&cand_cnt, cand);
		if (status != PJ_SUCCESS)
			return -status;

		/* And encode the candidates as SDP */
		for (j = 0; j<cand_cnt; ++j) {
			printed = print_cand(p, maxlen - (unsigned)(p - buffer), &cand[j]);
			if (printed < 0)
				return -PJ_ETOOSMALL;
			p += printed;
		}
	}

	if (p == buffer + maxlen)
		return -PJ_ETOOSMALL;

	*p = '\0';
	return (int)(p - buffer);
}


void Ice::InputSession(char *dataArr) {

	pj_thread_desc desc;
    pj_thread_t *this_thread;

	pj_thread_register("thread", desc, &this_thread);

	char linebuf[1024];
	unsigned media_cnt = 0;
	unsigned comp0_port = 0;
	char     comp0_addr[80];
	pj_bool_t done = PJ_FALSE;

	comp0_addr[0] = '\0';
	int index = 0;
	int dataLen = strlen(dataArr);

	while (!done) {
		pj_size_t len;
		char *line;

		if (index < dataLen){
			pj_bzero(linebuf, sizeof(linebuf));
			int start = index;
			int count = 0;
			while (*(dataArr + index) != '\n' && *(dataArr + index) != '\0'){
				char c = *(dataArr + index);
				index++;
				count++;
			}
			index++;
			count++;
			strncpy(linebuf, dataArr + start, count);
		}
		else
		{
			break;
			pj_bzero(linebuf, sizeof(linebuf));
			linebuf[0] = '\r';
			linebuf[1] = '\n';

		}


		len = strlen(linebuf);
		while (len && (linebuf[len - 1] == '\r' || linebuf[len - 1] == '\n'))
			linebuf[--len] = '\0';

		line = linebuf;
		while (len && pj_isspace(*line))
			++line, --len;

		if (len == 0)
			break;

		/* Ignore subsequent media descriptors */
		if (media_cnt > 1)
			continue;

		switch (line[0]) {
		case 'm':
		{
			int cnt;
			char media[32], portstr[32];

			++media_cnt;
			if (media_cnt > 1) {
				break;
			}

			cnt = sscanf(line + 2, "%s %s RTP/", media, portstr);
			if (cnt != 2) {
				goto on_error;
			}

			comp0_port = atoi(portstr);

		}
			break;
		case 'c':
		{
			int cnt;
			char c[32], net[32], ip[80];

			cnt = sscanf(line + 2, "%s %s %s", c, net, ip);
			if (cnt != 3) {
				goto on_error;
			}

			strcpy(comp0_addr, ip);
		}
			break;
		case 'p':
		{
			int cnt;
			
			char ps[10];
			cnt = sscanf(line + 2, "%s", ps);
			if (cnt != 1) {
				goto on_error;
			}
			
			m_pnum = atoi(ps);
		}
			break;

		case 'a':
		{
			char *attr = strtok(line + 2, ": \t\r\n");
			if (strcmp(attr, "ice-ufrag") == 0) {
				strcpy(rem.ufrag, attr + strlen(attr) + 1);
			}
			else if (strcmp(attr, "ice-pwd") == 0) {
				strcpy(rem.pwd, attr + strlen(attr) + 1);
			}
			else if (strcmp(attr, "rtcp") == 0) {
				char *val = attr + strlen(attr) + 1;
				int af, cnt;
				int port;
				char net[32], ip[64];
				pj_str_t tmp_addr;
				pj_status_t status;

				cnt = sscanf(val, "%d IN %s %s", &port, net, ip);
				if (cnt != 3) {
					goto on_error;
				}

				if (strchr(ip, ':'))
					af = pj_AF_INET6();
				else
					af = pj_AF_INET();

				pj_sockaddr_init(af, &rem.def_addr[1], NULL, 0);
				tmp_addr = pj_str(ip);
				status = pj_sockaddr_set_str_addr(af, &rem.def_addr[1],
					&tmp_addr);
				if (status != PJ_SUCCESS) {
					goto on_error;
				}
				pj_sockaddr_set_port(&rem.def_addr[1], (pj_uint16_t)port);

			}
			else if (strcmp(attr, "candidate") == 0) {
				char *sdpcand = attr + strlen(attr) + 1;
				int af, cnt;
				char foundation[32], transport[12], ipaddr[80], type[32];
				pj_str_t tmpaddr;
				int comp_id, prio, port;
				pj_ice_sess_cand cand;
				pj_status_t status;

				cnt = sscanf(sdpcand, "%s %d %s %d %s %d typ %s",
					foundation,
					&comp_id,
					transport,
					&prio,
					ipaddr,
					&port,
					type);
				if (cnt != 7) {
					goto on_error;
				}

				pj_bzero(&cand, sizeof(cand));

				if (strcmp(type, "host") == 0)
					cand.type = PJ_ICE_CAND_TYPE_HOST;
				else if (strcmp(type, "srflx") == 0)
					cand.type = PJ_ICE_CAND_TYPE_SRFLX;
				else if (strcmp(type, "relay") == 0)
					cand.type = PJ_ICE_CAND_TYPE_RELAYED;
				else {
					goto on_error;
				}

				cand.comp_id = (pj_uint8_t)comp_id;
				pj_strdup2(obj->ice_pool, &cand.foundation, foundation);
				cand.prio = prio;

				if (strchr(ipaddr, ':'))
					af = pj_AF_INET6();
				else
					af = pj_AF_INET();

				tmpaddr = pj_str(ipaddr);

				pj_sockaddr_init(af, &cand.addr, NULL, 0);
				status = pj_sockaddr_set_str_addr(af, &cand.addr, &tmpaddr);
				if (status != PJ_SUCCESS) {
					goto on_error;
				}

				pj_sockaddr_set_port(&cand.addr, (pj_uint16_t)port);

				rem.cand[rem.cand_cnt] = cand;

				++rem.cand_cnt;

				if (cand.comp_id > rem.comp_cnt)
					rem.comp_cnt = cand.comp_id;
			}
		}
			break;
		}
	}

	if (rem.cand_cnt == 0 ||
		rem.ufrag[0] == 0 ||
		rem.pwd[0] == 0 ||
		rem.comp_cnt == 0)
	{
		goto on_error;
	}

	if (comp0_port == 0 || comp0_addr[0] == '\0') {
		goto on_error;
	}
	else {
		int af;
		pj_str_t tmp_addr;
		pj_status_t status;

		if (strchr(comp0_addr, ':'))
			af = pj_AF_INET6();
		else
			af = pj_AF_INET();

		pj_sockaddr_init(af, &rem.def_addr[0], NULL, 0);
		tmp_addr = pj_str(comp0_addr);
		status = pj_sockaddr_set_str_addr(af, &rem.def_addr[0],
			&tmp_addr);
		if (status != PJ_SUCCESS) {
			goto on_error;
		}
		pj_sockaddr_set_port(&rem.def_addr[0], (pj_uint16_t)comp0_port);
	}

	return;

on_error:
	
	return;
}


Ice::Ice(LogWriter *log, WindowsEvent * outEvent,WindowsEvent * inEvent,P2pEventListener * p2pEventListener, int port)
{
	
	StaticInit();
	obj = 0;
	m_outEvent = outEvent;
	m_inEvent = inEvent;
	m_p2pEventListener = p2pEventListener;
	m_pnum = port;

}

void Ice::RegisterThread(pj_pool_t * pool){

    pj_thread_t *pthread = NULL;
    pj_thread_desc * pdesc ;
    pj_bool_t has_registered = PJ_FALSE;
    has_registered = pj_thread_is_registered();
    if(has_registered) return;
    
    pj_size_t sz =  sizeof(pj_thread_desc);
    pdesc = (pj_thread_desc *)pj_pool_alloc(pool,sz);
    
    if (pj_thread_register(NULL, *pdesc, &pthread) != PJ_SUCCESS) {
		// error
	}
}



void Ice::StaticInit(){
    int ret = -1;
   
	if (g_ice) {
		g_ice->sessions++;
		ret = 0;
		return;
	}

	pj_log_set_level(0);
	g_ice = &g_ice_stor;

	memset(g_ice, 0, sizeof(struct IceContext));

	

    do {
        

        ret = pj_init();
        if (ret != PJ_SUCCESS) {
            break;
        }
        g_ice->pj_inited = 1;
        
        // create pool factory
        pj_caching_pool_init(&g_ice->cp, NULL, 0);
        g_ice->cp_inited = 1;
        
        /* Create application memory pool */
        g_ice->pool = pj_pool_create(&g_ice->cp.factory, "ice_global_pool",
                                      512, 512, NULL);
        
        RegisterThread(g_ice->pool);
        
        ret = pjlib_util_init();
        if (ret != PJ_SUCCESS) {
            break;
        }
        g_ice->pjlib_inited = 1;
        
        ret = pjnath_init();
        if (ret != PJ_SUCCESS) {
            break;
        }
        g_ice->pjnath_inited = 1;
        ret = PJ_SUCCESS;
		
		g_ice->sessions = 1;

    } while (0);

    if(ret != PJ_SUCCESS){
    	//exit
    }

	return;

}
void Ice::StartNego()
{
	pj_str_t rufrag, rpwd;
	pj_status_t status;

	if (obj->icest == NULL) {
		return;
	}

	if (!pj_ice_strans_has_sess(obj->icest)) {
		return;
	}

	if (rem.cand_cnt == 0) {
		return;
	}

	status = pj_ice_strans_start_ice(obj->icest,
		pj_cstr(&rufrag, rem.ufrag),
		pj_cstr(&rpwd, rem.pwd),
		rem.cand_cnt,
		rem.cand);

}

void Ice::StopIce(){ obj->terminating = true; m_outEvent->notify();}

Ice::~Ice()
{

	obj->terminating = true;
	m_outEvent->notify();
	this->terminate();
	this->wait();

	pj_thread_desc desc;
    pj_thread_t *this_thread;
	pj_thread_register("thread", desc, &this_thread);

	Free();
	g_ice->sessions--;
	if(g_ice->sessions) return;
	if(!g_ice) return;
  
    if(g_ice->cp_inited){
        pj_caching_pool_destroy(&g_ice->cp);
        g_ice->cp_inited = 0;
    }

	if(g_ice->pj_inited){
		pj_shutdown();
		g_ice->pj_inited = 0;
	}

	g_ice = 0;


}

void Ice::Free(){
	if(!obj) return ;



    // stop strans
    if (obj->icest) {

    	// stop session
		if (pj_ice_strans_has_sess(obj->icest)) {
			pj_ice_strans_stop_ice(obj->icest);
		}

        pj_ice_strans_destroy(obj->icest);
        obj->icest = 0;
    }


	// stop thread
	obj->thread_quit_flag = PJ_TRUE;
	if (obj->ice_thread)
	{
		pj_thread_join(obj->ice_thread);
		pj_thread_destroy(obj->ice_thread);
		obj->ice_thread = 0;
	}


	// stop io queue
	if (obj->ice_cfg.stun_cfg.ioqueue)
	{
		pj_ioqueue_destroy(obj->ice_cfg.stun_cfg.ioqueue);
		obj->ice_cfg.stun_cfg.ioqueue = 0;
	}


	// stop timer
	if (obj->ice_cfg.stun_cfg.timer_heap)
	{
		pj_timer_heap_destroy(obj->ice_cfg.stun_cfg.timer_heap);
		obj->ice_cfg.stun_cfg.timer_heap = 0;
	}

	if (obj->lock)
	{
		pj_lock_destroy(obj->lock);
		obj->lock = 0;
	}


	// stop caching pool
	if(obj->cp_inited)
	{
		pj_caching_pool_destroy(&obj->cp);
		obj->cp_inited = 0;
	}

	free(obj);
}


void Ice::execute()
{
	pj_thread_desc desc;
	pj_thread_t *this_thread;
	pj_thread_register("thread", desc, &this_thread);


	Sctp::sctp_message * msg;

	while(!isTerminating())
	{
		if(obj->dtls_tr->handshake_done)
		{
			m_outEvent->waitForEvent();
			if(obj->terminating)
			{
				return;
			}

			while(!obj->sctp_tr->outgoing_queue.empty())
			{
				{
					AutoLock al(&obj->sctp_tr->outgoing_mutex);
					msg = obj->sctp_tr->outgoing_queue.front();
				}

				if(msg==NULL) continue;

				if (msg->len > 0) {
					{
						AutoLock al(&obj->dtls_tr->outgoing_mutex);
						SSL_write(obj->dtls_tr->ssl, msg->data, msg->len);
					}
					IO(obj);
				}

				{
					AutoLock al(&obj->sctp_tr->outgoing_mutex);
					obj->sctp_tr->outgoing_queue.pop();
				}

				Sctp::destroy_sctp_message(msg);
			}
		}
		else
		{
			struct timeval timeout;
			DTLSv1_get_timeout(obj->dtls_tr->ssl, &timeout);
			int timeout_value = timeout.tv_sec*1000 + timeout.tv_usec/1000;
			if(timeout_value == 0)
			{
				DTLSv1_handle_timeout(obj->dtls_tr->ssl);
				IO(obj);
			}
			sleep(100);
		}
	}
}


