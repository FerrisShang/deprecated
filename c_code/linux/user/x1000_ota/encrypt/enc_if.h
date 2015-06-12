#ifndef __ENC_IF_H__
#define __ENC_IF_H__

#include <linux/ioctl.h>
#include "base.h"

#define ENCRYPT_DEV "/dev/encrypt"

typedef enum{
	TYPE_ENCRYPT_NULL = 0,
	TYPE_ENCRYPT_RSA,
	TYPE_ENCRYPT_AES,
	TYPE_ENCRYPT = 0xFF,//use for init
}eEncrypt_t;

typedef enum{
	ENCRYPT_AES_UNINIT = 0,
	ENCRYPT_AES_INITED,
	ENCRYPT_AES_TIMEOUT,
}eEnc_state_t;

typedef struct{
	int fd;
	eEnc_state_t state;
}sEncrypt_t;

s32_t init_encrypt(sEncrypt_t *enc);
s32_t update_aes(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len);

s32_t encrypt_rsa(sEncrypt_t *enc, s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf);
s32_t decrypt_rsa(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf);
s32_t encrypt_aes(sEncrypt_t *enc, s8_t *pt_buf, s32_t pt_len, s8_t *ct_buf);
s32_t decrypt_aes(sEncrypt_t *enc, s8_t *ct_buf, s32_t ct_len, s8_t *pt_buf);

#endif /* __ENC_IF_H__ */
