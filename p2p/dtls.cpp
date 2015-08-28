

#include "dtls.h"


EVP_PKEY * Dtls::gen_key()
{
  EVP_PKEY *pkey = EVP_PKEY_new();
  BIGNUM *exponent = BN_new();
  RSA *rsa = RSA_new();
  if (!pkey || !exponent || !rsa ||
      !BN_set_word(exponent, 0x10001) || // 65537
      !RSA_generate_key_ex(rsa, 2048, exponent, NULL) ||
      !EVP_PKEY_assign_RSA(pkey, rsa)) {
    EVP_PKEY_free(pkey);
    BN_free(exponent);
    RSA_free(rsa);
    return NULL;
  }
  BN_free(exponent);
  return pkey;
}

X509 * Dtls::gen_cert(EVP_PKEY* pkey, const char *common, int days) {
  X509 *x509 = NULL;
  BIGNUM *serial_number = NULL;
  X509_NAME *name = NULL;

  if ((x509 = X509_new()) == NULL)
    return NULL;

  if (!X509_set_pubkey(x509, pkey))
    return NULL;

  ASN1_INTEGER* asn1_serial_number;
  if ((serial_number = BN_new()) == NULL ||
      !BN_pseudo_rand(serial_number, 64, 0, 0) ||
      (asn1_serial_number = X509_get_serialNumber(x509)) == NULL ||
      !BN_to_ASN1_INTEGER(serial_number, asn1_serial_number))
    goto cert_err;

  if (!X509_set_version(x509, 0L)) // version 1
    goto cert_err;

  if ((name = X509_NAME_new()) == NULL ||
      !X509_NAME_add_entry_by_NID(
          name, NID_commonName, MBSTRING_UTF8,
          (unsigned char*)common, -1, -1, 0) ||
      !X509_set_subject_name(x509, name) ||
      !X509_set_issuer_name(x509, name))
    goto cert_err;

  if (!X509_gmtime_adj(X509_get_notBefore(x509), 0) ||
      !X509_gmtime_adj(X509_get_notAfter(x509), days * 24 * 3600))
    goto cert_err;

  if (!X509_sign(x509, pkey, EVP_sha1()))
    goto cert_err;

  if (0) {
cert_err:  
    X509_free(x509);
    x509 = NULL;
  }
  BN_free(serial_number);
  X509_NAME_free(name);

  return x509;
}

static int
verify_peer_certificate_cb(int ok, X509_STORE_CTX *ctx)
{
  return 1;
}



Dtls::Dtls(LogWriter *log, dtls_transport * dtls) {
	
SSL_library_init();
OpenSSL_add_all_algorithms();

SSL_CTX *context = SSL_CTX_new(DTLSv1_method());

if (context == NULL)
    return;

if (SSL_CTX_set_cipher_list(context, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH") != 1)
	return;

SSL_CTX_set_read_ahead(context, 1); // for DTLS
SSL_CTX_set_verify(context, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_peer_certificate_cb);

EVP_PKEY *key = gen_key();
if (key == NULL)
    return;

SSL_CTX_use_PrivateKey(context, key);

X509 *cert = gen_cert(key, "vnc", 365);
if (cert == NULL)
     return;

SSL_CTX_use_certificate(context, cert);
  
if (SSL_CTX_check_private_key(context) != 1)
   return;

unsigned int len;
unsigned char buf[32676];
X509_digest(cert, EVP_sha256(), buf, &len);

for (unsigned int i = 0; i < len; ++i) {
	printf("%02X:", buf[i]);
}


//dtls = (struct dtls_transport *)calloc(1, sizeof *dtls);
  if (dtls == NULL)
    return;
dtls->handshake_done = FALSE;

 SSL *ssl = SSL_new(context);
 if (ssl == NULL)
    return;
  dtls->ssl = ssl;

  BIO *bio = BIO_new(BIO_s_mem());
  if (bio == NULL)
     return;
  BIO_set_mem_eof_return(bio, -1);
  dtls->incoming_bio = bio;

  bio = BIO_new(BIO_s_mem());
  if (bio == NULL)
        return;
  BIO_set_mem_eof_return(bio, -1);
  dtls->outgoing_bio = bio;

  SSL_set_bio(dtls->ssl, dtls->incoming_bio, dtls->outgoing_bio);

  EC_KEY *ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  SSL_set_options(dtls->ssl, SSL_OP_SINGLE_ECDH_USE);
  SSL_set_tmp_ecdh(dtls->ssl, ecdh);
  EC_KEY_free(ecdh);

}

Dtls::~Dtls(){

}

