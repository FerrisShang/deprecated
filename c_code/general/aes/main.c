#include "stdio.h"
#include "aes.h"
int main(int argc, char *argv[])
{
	aes_context     ctx;
	unsigned char key[16] = {0};
	unsigned char in[16] = {0};
	unsigned char out[16] = {0};
	aes_set_key(key, 16, &ctx);                                                         
	aes_encrypt(in, out, &ctx);  /* outputs in byte 48 to byte 63 */
	printf("key: %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			key[0],key[1],key[2],key[3],key[4],key[5],key[6],key[7],
			key[8],key[9],key[10],key[11],key[12],key[13],key[14],key[15]);
	printf("in : %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			in[0],in[1],in[2],in[3],in[4],in[5],in[6],in[7],
			in[8],in[9],in[10],in[11],in[12],in[13],in[14],in[15]);
	printf("out: %02x %02x %02x %02x %02x %02x %02x %02x "
			"%02x %02x %02x %02x %02x %02x %02x %02x\n",
			out[0],out[1],out[2],out[3],out[4],out[5],out[6],out[7],
			out[8],out[9],out[10],out[11],out[12],out[13],out[14],out[15]);
	return 0;
}
