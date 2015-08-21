// Copyright (C) den


#ifndef _DTLS_H_
#define _DTLS_H_

#include "log-writer/LogWriter.h"
#include "thread/LocalMutex.h"


#define NOCRYPT
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

class Dtls
{
public:

struct dtls_transport {
  SSL *ssl;
  BIO *incoming_bio;
  BIO *outgoing_bio;
  LocalMutex incoming_mutex;
  LocalMutex outgoing_mutex;
  bool handshake_done;
  
 };


	Dtls(LogWriter *log, dtls_transport * dtls);
	~Dtls();

private:
	static EVP_PKEY * gen_key();
	static X509 *     gen_cert(EVP_PKEY* pkey, const char *common, int days);

};

#endif // _DTLS_H_