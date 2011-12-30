#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>

#define BKPUK_DEFAULT_KEYLEN 1024

#define MAX_ERROR_STRING

char error_buffer[MAX_ERROR_STRING];
char *GenErrorStr()
{
	memset(error_buffer, '\0', );
}

int GenKeyPair(unsigned char **pPubKey, int *pPubKeyLen, 
		   unsigned char **pPriKey, int *pPriKeyLen, int mod)
{
}

int PublicKeyEncrypt()
{
}

int PrivateKeyDecrypt()
{
}

int BCPubKeyGetKey(unsigned char *pPubKeyOut)
{
	RSA *pRsa = NULL;
	unsigned char cPubKey[BKPUK_DEFAULT_KEYLEN] = {0};
	unsigned char *pPubKey = NULL;
	unsigned char cPriKey[BKPUK_DEFAULT_KEYLEN] = {0};
	unsigned char *pPriKey = NULL;
	int nPubKeyLen = 0;
	int nPriKeyLen = 0;
	int ret;

	pPubKey = calloc(1, BKPUK_DEFAULT_KEYLEN);
	pPriKey = calloc(1, BKPUK_DEFAULT_KEYLEN);

	pRsa = RSA_generate_key(BKPUK_DEFAULT_KEYLEN, 17, NULL, NULL);

	if (pRsa = NULL)
		goto failed;

	nPriKeyLen = i2d_RSAPrivateKey(pRsa, &pPriKey);

	nPubKeyLen = i2d_RSAPublicKey(pRsa, &pPubKey);

	memcpy((void *)pPubKeyOut, (void *)cPubKey, nPubKeyLen+1);

	return nPubKeyLen;

failed:
	ERR_error_string_n(ERR_get_error(), );
	return -1;
}

int main(int argc, char *argv[])
{
	char *buf;
	int ret;

	buf = calloc(1, BKPUK_DEFAULT_KEYLEN);

	ret = BCPubKeyGetKey(buf);

	printf("return key len %d bytes:%s\n", ret, buf);

	return 0;
}
