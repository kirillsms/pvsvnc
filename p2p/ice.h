// Copyright (C) den


#ifndef _ICE_H_
#define _ICE_H_

#include "log-writer/LogWriter.h"

#include "thread/Thread.h"
#include "thread/GlobalMutex.h"
#include "thread/AutoLock.h"
#include "win-system/WindowsEvent.h"

#include "pj/addr_resolv.h"
#include <pjlib.h>
#include <pjlib-util.h>
#include <pjnath.h>

#include "dtls.h"
#include "sctp.h"
#include "P2pEventListener.h"

class Ice:
	public Thread
{
public:
  Ice(LogWriter *log, WindowsEvent * outEvent,WindowsEvent * inEvent,P2pEventListener * p2pEventListener, int port);
  ~Ice();
  bool Init(bool isControl, Dtls::dtls_transport * dtls_transport, Sctp::sctp_transport * sctp_transport);
  int EncodeSession(char buffer[], unsigned maxlen);
  void InputSession(char *dataArr);
  void StartNego();
  void StopIce();

  struct IceContext
	{
		int pj_inited;
		int pjlib_inited;
		int pjnath_inited;
		int pjlog_installed;

		pj_caching_pool cp;
		int cp_inited;
		pj_pool_t * pool;
		int sessions;
	};
  

static struct IceContext g_ice_stor;
static struct IceContext * g_ice;

int m_pnum;

private:

struct RemInfo
	{
		char		 ufrag[80];
		char		 pwd[80];
		unsigned	 comp_cnt;
		pj_sockaddr	 def_addr[PJ_ICE_MAX_COMP];
		unsigned	 cand_cnt;
		pj_ice_sess_cand cand[PJ_ICE_ST_MAX_CAND];
	} rem;



struct IceSt
	{
		// ice instance
		pj_caching_pool cp;
		int cp_inited;

		pj_pool_t *ice_pool;

		pj_lock_t	    *lock;

		pj_ice_strans_cfg ice_cfg;
		pj_ice_strans *icest;
		pj_ice_sess_role role;

		int ice_init_done;
		pj_status_t ice_init_result;

		int ice_nego_done;
		pj_status_t ice_nego_status;

		// callback thread.
		pj_thread_t *ice_thread;
		char thread_quit_flag;
		
		Dtls::dtls_transport * dtls_tr;
		Sctp::sctp_transport * sctp_tr;

		RemInfo * rem_inf;
		bool terminating;
		P2pEventListener * p2pEventListener;
		char err_str[1024];
		
	};
	typedef struct IceSt * ice_t;

	static int WorkerThread(void *arg);
	void StaticInit();
	int print_cand(char buffer[], unsigned maxlen, const pj_ice_sess_cand *cand);
	static pj_status_t HandleEvents(ice_t obj, unsigned max_msec, unsigned *p_count);
	static void OnIceComplete(pj_ice_strans *ice_st, pj_ice_strans_op op, pj_status_t status);
	static void OnReceiveData(pj_ice_strans* ice_st, unsigned comp_id, void* pkt, pj_size_t size, const pj_sockaddr_t* src_addr, unsigned src_addr_len);
	static void RegisterThread(pj_pool_t * pool);
	void Free();
	static void IO(ice_t ice_obj);
	ice_t obj;
	static const char* kStunServer;
	WindowsEvent * m_outEvent;
	WindowsEvent * m_inEvent;
	P2pEventListener * m_p2pEventListener;
	

protected:
	virtual void execute();



};



#endif // _ICE_H_