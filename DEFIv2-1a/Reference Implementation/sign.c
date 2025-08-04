#include <stdio.h>
#include <stdlib.h>

#include "defiv2_keygen.h"
#include "defiv2_siggen.h"
#include "defiv2_sigver.h"

#define CRYPTO_SECRETKEYBYTES 426
#define CRYPTO_PUBLICKEYBYTES 515
#define CRYPTO_BYTES 483

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
{
	return key_gen(pk, sk);
}

int crypto_sign(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long mlen, const unsigned char *sk)
{
	return sig_gen(sm, smlen, m, mlen, sk);
}

int crypto_sign_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long smlen, const unsigned char *pk)
{
	return sig_ver(m, mlen, sm, smlen, pk);
}

