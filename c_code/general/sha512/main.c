#include <stdio.h>
#include <string.h>
#include "sha512.h"
int main(int argc, char *argv[])
{
	char *p = "sha-512";
	unsigned char out[80];
	mbedtls_sha512(p, strlen(p), out, 0);
	int i;
	for(i=0;i<64;i++){
		if((i%16)==0)printf("\n");
		printf("%02x ", out[i]);
	}
	printf("\n");
	return 0;
}
